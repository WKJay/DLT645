Import('RTT_ROOT')
Import('rtconfig')
from building import *

cwd = GetCurrentDir()
src = Glob("src/*.c")

# add dlt645 sample
if GetDepend('DLT645_USING_SAMPLE'):
    src = src + ['sample/dlt645_sample.c']
if (GetDepend('DLT645_USING_CUSTOM_PORT') == False):
    src = src + ['port/dlt645_port.c']

CPPPATH = [cwd + '/inc']
CPPPATH += [cwd + '/port']

group = DefineGroup('dlt645', src, depend = ['PKG_USING_DLT645'], CPPPATH = CPPPATH)

Return('group')
