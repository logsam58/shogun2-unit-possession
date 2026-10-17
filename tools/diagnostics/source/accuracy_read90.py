"""Decode the optional numeric Test90 channel; never follows game pointers."""
import struct
SIZE=112
FORMAT='<4Ii7IQ8f5I4x'
assert struct.calcsize(FORMAT)==SIZE
def decode_accuracy90(data,pid):
 if len(data)!=SIZE:return {'unavailable':True,'error':'size'}
 values=struct.unpack(FORMAT,data)
 if values[:4]!=(0x30434153,90,SIZE,pid):return {'unavailable':True,'error':'protocol','header':list(values[:4])}
 names=['magic','version','size','pid','sequence','lease','actor','unit','record','valid','error','updates','tick_ms','half_angle','horizontal','vertical','chance','calibration','core','control','projection_y','rts_captures','rts_restores','rts_rejects','rts_saved','simulation_tick']
 return dict(zip(names,values))
