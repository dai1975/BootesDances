# -*- coding: utf-8 -*-

import socket
import os
import copy

CWD = os.getcwd()

def get_config():
   g = {}
   l = {}
   execfile(os.path.join(CWD, 'SConstruct.cf'), g, l)
   cfg = l['create_config']()
   return cfg

def create_envs(conf):
   def create_env(conf):
      import SCons.Tool.MSCommon.vc
      #print SCons.Tool.MSCommon.vc.cached_get_installed_vcs()
      e = Environment(tools=['default'])
      if conf.vcver is not None:
         e['MSVC_VERSION'] = conf.vcver
         e['MSVS_VERSION'] = conf.vcver
      vc = SCons.Tool.MSCommon.vc.get_default_version(e)
      vcdir = SCons.Tool.MSCommon.vc.find_vc_pdir(vc)
      vcdir = os.path.normpath(vcdir)
      vsdir = os.path.dirname(vcdir)
      env = os.environ
      env2 = {
         'MSVC_VERSION': vc,
         'PATH': ';'.join([
               os.path.join(vcdir, 'BIN'),
               os.path.join(vsdir, 'Common7', 'IDE'),
               ]),
         'INCLUDE': os.path.join(vcdir, 'INCLUDE'),
         'LIB': os.path.join(vcdir, 'LIB'),
         'TMP': os.environ['TMP'],
         }
      for k in env2.iterkeys(): env[k] = env2[k]
      e = SCons.Environment.Environment(ENV=env, tools=['default','msvc'])
      e['MSVS']['VCINSTALLDIR'] = vcdir #why is not it set automatically?
      e['CPPDEFINES'] = []
      e['CPPPATH'] = []
      e['LIBPATH'] = []
      e['LIBS'] = []
      return e
   envs = {}
   envs['release'] = create_env(conf)
   envs['release']['CPPDEFINES'] += ['NDEBUG']
   envs['release']['CCFLAGS']    += ['/O2', '/MT', '/wd4819']
   envs['release']['LINKFLAGS']  += ['/wd4099']

   envs['debug'] = create_env(conf)
   envs['debug']['CCFLAGS']    += ['/Od', '/MTd', '/RTC1', '/wd4819']
   envs['debug']['CCPDBFLAGS']  = '/Zi /Fd${TARGET}.pdb'
   envs['debug']['PDB']         = '${TARGET}.pdb'
   envs['debug']['LINKFLAGS']  += ['/wd4099']

   for k in envs.keys():
      #envs[k]['CCFLAGS'] += ['/Za', '/EHsc', '/nologo']
      envs[k]['CCFLAGS'] += ['/EHsc', '/nologo']
   return envs

def get_all_targets():
   return ['wiimote', 'DirectShow', 'DirectShowTexture', 'BootesLib', 'BootesDances']

def get_targets():
   rets = {}
   subs = Glob('*/SConscript')
   for sub in subs:
      subfile = sub.__str__()
      subdir  = os.path.dirname(subfile)
      rets[subdir] = subfile
   return rets

def get_depends(subdir):
   if subdir == 'BootesDances': return ['DirectShowTexture', 'BootesLib', 'wiimote']
   elif subdir == 'BootesLib': return []
   elif subdir == 'DirectShowTexture': return ['DirectShow']
   elif subdir == 'DirectShow': return []
   elif subdir == 'wiimote': return []
   else:
      return []

cfg  = get_config()
envs = create_envs(cfg)
dirs = get_targets()

env_keys = []
dir_keys = []
others   = []
all_dir  = False
for t in COMMAND_LINE_TARGETS:
   if t in envs:   env_keys.append(t)
   elif t in dirs: dir_keys.append(t)
   elif t == 'all': all_dir = True
   else: others.append(t)
if all_dir:
   dir_keys = get_all_targets()

if 0 < len(others):
   print "Error: unknown target or build: %s" % ( ', '.join(others))
   exit

if len(env_keys) == 0:
   print "Error: select build: %s" % ( ', '.join(envs.keys()))
   exit
if len(dir_keys) == 0:
   print "Error: select target: %s" % ( ', '.join(dirs.keys()))
   exit

PROJECT = 'BootesDances'
ek_targets = {}
for dk in dir_keys:
   for ek in env_keys:
      t    = '%s-%s' % (dk,ek)
      env  = envs[ek]
      wdir = '%s/%s/%s/%s' % (cfg.WorkDir, ek, PROJECT, dk)
      #odir = '%s/%s/%s/%s' % (cfg.OutDir, ek, PROJECT, dk)
      odir = '%s/%s' % (cfg.OutDir, ek)
      config = copy.copy(cfg)
      config.WorkDir = wdir
      config.OutDir  = odir
      config.Build   = ek
      sc = SConscript(dirs[dk], exports=['env','config'],variant_dir=wdir,duplicate=False)
      Alias(t, sc)
      Depends(ek, t)
      Depends(dk, t)
      if ek_targets.has_key(ek):
         ek_targets[ek] += [t]
      else:
         ek_targets[ek] = [t]
      #subs[dk][ek] = sc
      pass
   pass

for ek in ek_targets:
   Alias(ek, ek_targets[ek])

#Depends('all', get_all_targets())
Alias('all', get_all_targets())

"""
for dk in subs.keys():
   depends = get_depends(dk)
   for dep in depends:
      if dep in subs:
         for ek in subs[dk].keys():
            for a,b in zip(subs[dk][ek], subs[dep][ek]):
               Depends(a,b)
"""

# Local Variables:
# mode: python
# coding: utf-8-dos
# indent-tabs-mode: nil
# tab-width: 3
# End:
