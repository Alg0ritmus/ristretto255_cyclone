import os
import glob
from datetime import datetime

HEADDER_SIZE = 700 # in characters

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
for file_path in arr:
    with open(file_path, 'r') as file:
        content = file.read()

    # HANDLE VERSION
    def increment_patch(version):
        major, minor, patch = map(int, version.split('.'))
        new_patch = patch + 1
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




