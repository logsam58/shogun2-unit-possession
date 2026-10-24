"""Operator-only reversible full Workshop test. Players do not run this tool."""
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
import stage_probe164 as stage
stage.NAME='shogun2_unit_possession_workshop165.pack'
stage.LINE='mod '+stage.NAME+';'
stage.TEST=165
if __name__=='__main__':stage.main()
