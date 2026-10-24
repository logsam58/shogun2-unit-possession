"""Isolated operator-tool fixtures. Never inspect or modify a real game/profile."""
import argparse
import contextlib
import hashlib
import io
import json
from pathlib import Path
import sys
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import stage_probe164 as stage


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--candidate',type=int,choices=[164,165],default=164)
    args=parser.parse_args();output=args.output.resolve()
    if args.candidate==165:import stage_workshop165
    allowed=Path(__file__).resolve().parents[2]/('artifacts/Test'+str(args.candidate))
    if output.exists() or not output.is_relative_to(allowed):
        parser.error('Use a new fixture output under artifacts/Test164')
    output.mkdir(parents=True)
    loader=b'owned synthetic loader fixture';expected=hashlib.sha256(loader).hexdigest()
    rows=[]
    def invoke(*args):
        with patch.object(sys,'argv',['stage_probe164.py',*map(str,args)]),contextlib.redirect_stdout(io.StringIO()):
            stage.main()
    def refused(fn):
        try:fn()
        except ValueError:return
        raise AssertionError('Expected safe refusal')
    def fixture(label,encoding='utf-8',exclude=True):
        root=output/label;game=root/'game';(game/'data').mkdir(parents=True)
        selection=root/'profile/scripts/user.script.txt';selection.parent.mkdir(parents=True)
        text='# original\r\nlog_lua_errors_to_file;\r\n'
        encoded=(b'\xfe\xff'+text.encode(encoding)) if encoding=='utf-16-be' else text.encode(encoding)
        selection.write_bytes(encoded);(game/'dinput8.dll').write_bytes(loader)
        build=root/'build';build.mkdir();raw=b'private synthetic pack'
        (build/stage.NAME).write_bytes(raw)
        (build/'verification.json').write_text(json.dumps({'test':stage.TEST,'game_changed':False,'public_package_ready':False,'pack_sha256':stage.sha(raw)}))
        journal=root/'journal'
        flags=['--exclude-legacy-loader'] if exclude else []
        if stage.TEST==165:flags+=['--verification',build/'verification.json']
        invoke('prepare','--journal',journal,'--probe',build/stage.NAME,'--game',game,'--selection',selection,'--lua-logging',*flags)
        return game,selection,journal
    def snapshots(paths):return [p.read_bytes() if p.exists() else None for p in paths]
    with patch.object(stage,'require_game',lambda _:None),patch.object(stage,'game_pids',lambda:[]),patch.object(stage,'LEGACY_LOADER_SHA',expected):
        for encoding in ['utf-8','utf-16','utf-16-be']:
            for exclude in [False,True]:
                game,selection,journal=fixture(encoding+'-'+str(exclude),encoding,exclude)
                original=selection.read_bytes();invoke('stage','--journal',journal,'--closed')
                assert (not (game/'dinput8.dll').exists())==exclude
                if exclude:assert (journal/'excluded-dinput8.dll').read_bytes()==loader
                suffix_encoding='utf-16-le' if encoding=='utf-16' else encoding
                with selection.open('ab') as stream:stream.write('# later comment\r\n'.encode(suffix_encoding))
                invoke('rollback','--journal',journal)
                assert (game/'dinput8.dll').read_bytes()==loader
                decoded=stage.decode(selection.read_bytes())[0]
                assert 'later comment' in decoded and stage.LINE not in decoded
                assert 'log_lua_errors_to_file;' in decoded and 'log_lua_output_to_file;' not in decoded
                assert not (game/'data'/stage.NAME).exists()
                rows.append({'encoding':encoding,'exclude_legacy':exclude,'stage_rollback_comment_and_prior_flag':True})
        game,selection,journal=fixture('running-refusal')
        inputs=[game/'dinput8.dll',selection,game/'data'/stage.NAME]
        before=snapshots(inputs)
        with patch.object(stage,'game_pids',lambda:[123]):refused(lambda:invoke('stage','--journal',journal,'--closed'))
        assert snapshots(inputs)==before;rows.append({'running_refusal_preserves_all':True})
        game,selection,journal=fixture('changed-loader-before-stage')
        (game/'dinput8.dll').write_bytes(b'foreign before staging')
        paths=[game/'dinput8.dll',journal/'previous-dinput8.dll',selection,game/'data'/stage.NAME]
        before=snapshots(paths);refused(lambda:invoke('stage','--journal',journal,'--closed'));assert snapshots(paths)==before
        rows.append({'changed_loader_before_stage_refused_without_changes':True})
        game,selection,journal=fixture('partial-failure');original=selection.read_bytes();real_atomic=stage.atomic
        def fail_selection(path,raw):
            if path==selection:raise OSError('injected selection write failure')
            real_atomic(path,raw)
        try:
            with patch.object(stage,'atomic',fail_selection):invoke('stage','--journal',journal,'--closed')
        except OSError:pass
        else:raise AssertionError('Injected failure was not exercised')
        assert (game/'dinput8.dll').read_bytes()==loader and selection.read_bytes()==original
        assert not (game/'data'/stage.NAME).exists()
        rows.append({'partial_stage_failure_restores_loader_pack_selection':True})
        game,selection,journal=fixture('foreign-target');invoke('stage','--journal',journal,'--closed')
        target=game/'dinput8.dll';target.write_bytes(b'foreign replacement')
        paths=[target,journal/'excluded-dinput8.dll',selection,game/'data'/stage.NAME]
        before=snapshots(paths);refused(lambda:invoke('rollback','--journal',journal));assert snapshots(paths)==before
        rows.append({'foreign_dinput_preserved_without_other_rollback_changes':True})
        # Remove only the exact synthetic file created immediately above, then finish its rollback.
        assert target.parent.resolve()==game.resolve() and target.read_bytes()==b'foreign replacement'
        target.unlink();invoke('rollback','--journal',journal);assert target.read_bytes()==loader
        game,selection,journal=fixture('foreign-excluded');invoke('stage','--journal',journal,'--closed')
        excluded=journal/'excluded-dinput8.dll';excluded.write_bytes(b'foreign edit')
        paths=[game/'dinput8.dll',excluded,selection,game/'data'/stage.NAME]
        before=snapshots(paths);refused(lambda:invoke('rollback','--journal',journal));assert snapshots(paths)==before
        excluded.write_bytes(loader);invoke('rollback','--journal',journal)
        rows.append({'foreign_excluded_loader_preserved_without_other_changes':True})
    result={'checks':rows,'real_game_or_profile_changed':False,'engine_requirement_and_process_provider_mocked_in_fixtures_only':True}
    (output/'verification.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__=='__main__':main()
