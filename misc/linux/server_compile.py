import re
import subprocess
import os
import glob
import sys

current_path=os.path.dirname(os.path.realpath(__file__))

project_full_path = os.path.dirname(os.path.dirname(current_path))
deploy_path = project_full_path + '/buid'


build_files = glob.glob(current_path + '/build/*')
for f in build_files:
    os.remove(f)

gcc_compile_opts = "-c -Wall"

gcc_preprocessor_defs = " ".join(str(x) for x in \
				['-DARCH_STRING="linux"',
				'-DBOTLIB',
				'-DUSE_CURL',
				'-DUSE_CURL_DLOPEN',
				'-DUSE_OPENAL',
				'-DUSE_OPENAL_DLOPEN',
				'-DUSE_VOIP',
				'-DOPUS_BUILD',
				'-DHAVE_LRINTF',
				'-DFLOATING_POINT',
				'-DUSE_ALLOCA',
				'-DUSE_INTERNAL_JPEG',
				'-DUSE_STATIC_MODS',
				'-DQAGAME',
				'-DSERVER',
				'-DUSE_LOCAL_HEADERS'])
				
gcc_include = " ".join(str(x) for x in \
				['-I'+project_full_path + '/code',
				'-I'+project_full_path + '/code/SDL2/include',
				'-I'+project_full_path + '/code/libcurl-7.35.0',
				'-I'+project_full_path + '/code/AL',
				'-I'+project_full_path + '/code/opus-1.1/include',
				'-I'+project_full_path + '/code/opus-1.1/celt',
				'-I'+project_full_path + '/code/opus-1.1/silk',
				'-I'+project_full_path + '/code/opus-1.1/silk/float',
				'-I'+project_full_path + '/code/jpeg-8c',
				'-I'+project_full_path + '/code/zlib'])
				
gcc_str = 'gcc' + ' ' + gcc_compile_opts + ' ' +  gcc_preprocessor_defs + ' ' + gcc_include

failed = False

with open("../msvc12/server.vcxproj") as f:
	
	lines = []
	for line in f:
		lines = lines + [line]
	lines = lines + [
		"<ClCompile Include=\"..\\..\\code\\asm\\ftola.c\" />",
		"<ClCompile Include=\"..\\..\\code\\asm\\snapvector.c\" />"
	]
	for line in lines:
		if "ClCompile Include=\"" in line:
			m = re.search('ClCompile Include=\"(.+?)"', line)			
			c_path = m.group(1)
			path_arr = c_path.split('\\')
			c_file = path_arr[-1]
			if c_file == 'sys_win32.c':
				c_file = 'sys_unix.c'
				c_path = c_path.replace('sys_win32.c', 'sys_unix.c')
			obj_file = c_file.replace('.c', '.o')
			obj_path = "./build/" + obj_file
			run_str = gcc_str + ' ' + c_path + ' ' + "-o " + obj_path + ""
			run_str = run_str.replace('\\', '/')
			print obj_file
			#print run_str.split(' ')			
			process = subprocess.Popen(run_str.split(' '))
			#process = subprocess.Popen(['gcc', '-Wall', '../../code/botlib/be_aas_bspq3.c'])
			process.wait()
			if process.returncode != 0:
				print "FAILED"
				failed = true
				break

			
if failed:
	sys.exit(1)

print "Archiving object files to server.a"
print current_path+'/build'
process = subprocess.Popen('ar cr server.a *.o', shell=True, cwd=current_path+'/build')
process.wait()
if process.returncode != 0:
	print "FAILED"
	sys.exit(1)

print "Compiling to server"
process = subprocess.Popen('gcc ./server.a -o ./server -lm -ldl', shell=True, cwd=current_path+'/build')
process.wait()
if process.returncode != 0:
	print "FAILED"
	sys.exit(1)
	
print "Deploy..."
process = subprocess.Popen('mkdir -p ' + deploy_path + '; cp ./build/server ' + deploy_path + '/server', shell=True)
process.wait()
if process.returncode != 0:
	print "FAILED"
	sys.exit(1)
	
print "Ready in " + deploy_path


#f.close()