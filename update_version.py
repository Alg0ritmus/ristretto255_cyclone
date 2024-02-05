import os
import glob
from datetime import datetime



HEADDER_SIZE = 700 # in characters
GITHUB_ACTIVE = True # perform version check based on github status

# Get the script's directory
script_directory = os.path.dirname(os.path.abspath(__file__))

# Search for all files in the script's directory and its subdirectories
file_list = glob.glob(os.path.join(script_directory, '**/*'), recursive=True)

arr = []
for filepath in file_list:
    filename=filepath.split('\\')[-1]
    extension=filename.split('.')
    if len(extension)>1:
        if extension[1] in ['h','c']:
            arr.append(filepath)

# If u wanna exclude some files, just set 0 in tuple
# NOTE that if u add new .o/.c files u need to include  
# them manually into INCLUDED_FILES
# Also NOTE that by excluding files, u only prevent them
# to update date, version will be updated anyways bcs. of 
# update policy:
# Postup updatovania suborov:
# Pri akychkolvek upravach sa bude updatovat verzia v kazdom subore (aj nedotknutom), no datum sa
# bude upravovat len v suboroch, kt. boli akokolvek zmenene.

INCLUDED_FILES = [ # header will be changed in files where flag is se to 1
    (0,'config.h'),
    (0,'gf25519.c'),
    (0,'gf25519.h'),
    (0,'helpers.h'),
    (0,'main.c'),
    (0,'modl.c'),
    (0,'modl.h'),
    (0,'ristretto255.c'),
    (0,'ristretto255.h'),
    (0,'ristretto255_constants.h'),
    (0,'test_config.h'),
    (0,'utils.c'),
    (0,'utils.h'),
]

# Or, experimentally, if you are using github, INCLUDED_FILES can be set
# automatically with 'git status'

if GITHUB_ACTIVE:
    import subprocess

    cmd_command = 'git status -s'
    result = subprocess.run(cmd_command, stdout=subprocess.PIPE, text=True, shell=True)
    r = result.stdout.split()
    for i in r:
        if ('.c' in i) or ('.h' in i):
            for file_index,file in enumerate(INCLUDED_FILES): # set 1 if github sees changes in .c/.h file
                if i == file[1]:
                    INCLUDED_FILES[file_index] = (1,file[1])
            if i not in [f[1] for f in INCLUDED_FILES]:
                INCLUDED_FILES.append((1,i)) # if github sees change in file, that is new or not included in INCLUDED_FILES yet

for i,file_path in enumerate(arr):
    with open(file_path, 'r') as file:
        content = file.read()

    # HANDLE VERSION
    def increment_patch(version):
        try:
            major, minor, patch = map(int, version.split('.'))
        except:
            major, minor, patch = map(str, version.split('.'))
        new_patch = int(patch) + 1
        major, minor = 'T','T'
        new_version = f"{major}.{minor}.{new_patch}"
        return new_version

    new_version = ''
    version_start_index = content.find('version')
    version_end_index = content[version_start_index:].find('-')
    if version_start_index != -1 and version_end_index != -1:
        old_version = content[version_start_index:version_start_index+version_end_index].split()[1]
        new_version = increment_patch(old_version)

    # HANDLE DATE
    current_date = datetime.now()
    new_date = current_date.strftime('%d-%m-%Y')
    if INCLUDED_FILES[i][0] == 0:
        HEADDER_SIZE_UNTIL_DATE = 591 
        old_date = content[HEADDER_SIZE_UNTIL_DATE:][:10]
        new_date = old_date

    new_block = ''

    new_block = f'''// ******************************************************************
// ----------------- TECHNICAL UNIVERSITY OF KOSICE -----------------
// ---Department of Electronics and Multimedia Telecommunications ---
// -------- FACULTY OF ELECTRICAL ENGINEERING AND INFORMATICS -------
// ------------ THIS CODE IS A PART OF A MASTER'S THESIS ------------
// ------------------------- Master thesis --------------------------
// -----------------Patrik Zelenak & Milos Drutarovsky --------------
// ---------------------------version {new_version} -------------------------
// --------------------------- {new_date} ---------------------------
// ******************************************************************
'''

    # write changes
    content = new_block + content[HEADDER_SIZE:] 

    with open(file_path, 'w') as file:
        file.write(content)




