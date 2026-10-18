#include "playable_runtime.cpp"
int main(){try{
 Snapshot s{};s.active=true;s.serial=1;s.result=Result::Pending;
 for(unsigned i=0;i<1000;++i){s.response_elapsed=double(i/5)*.01;record_presented107(s,100.+double(i/5)*.01,false);}
 require(unique_frames107==200&&moving_frames107==200&&repeated_frames107==0,"CPU polling repeats inflated native marker counters");
 for(unsigned i=0;i<200;++i){s.response_elapsed=2.+double(i/4)*.01;record_presented107(s,103.+i*.01,false);}
 require(unique_frames107==400&&moving_frames107==250&&repeated_frames107==150&&max_marker_hold_us107>=29999&&max_marker_hold_us107<=30001,"Actual repeated native images were hidden");
 auto repeated=repeated_frames107;auto held=max_marker_hold_us107;
 for(unsigned i=0;i<100;++i){s.paused=true;record_presented107(s,106.+i*.01,false);}s.paused=false;
 for(unsigned i=0;i<100;++i)record_presented107(s,108.+i*.01,true);
 require(unique_frames107==600&&repeated_frames107==repeated&&max_marker_hold_us107==held,"Pause/click freeze reported as marker stutter");
 auto unique=unique_frames107;record_presented107(s,108.,false);record_presented107(s,NAN,false);require(unique_frames107==unique,"Invalid/old presentation counted");
 ++s.serial;record_presented107(s,110.,false);require(moving_frames107==251&&repeated_frames107==repeated,"New QTE inherited marker hold");
 std::cout<<"PASS: production cadence107 separates1000 CPU acknowledgement polls from200 native submissions, identifies150 true repeated images, excludes pause/click freeze and rejects invalid/old timestamps. Software submission evidence only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
