#!/usr/bin/env python3
import os
import sys
import shutil

TEMPLATE_ROOT = os.path.join("RenderingEngine", "Scripts", "Templates")
DEST_ROOT = os.path.join("TestApplications")

def copy_template(src_dir, dst_dir, project_name):
    for root, dirs, files in os.walk(src_dir):
        rel_path = os.path.relpath(root, src_dir)
        target_root = os.path.join(dst_dir, rel_path)
        os.makedirs(target_root, exist_ok=True)

        for file in files:
            src_file = os.path.join(root, file)
            dst_file = os.path.join(target_root, file)
            if ".in" in file:
                dst_file = os.path.join(target_root, file.replace(".in", ""))

                with open(src_file, "r") as f:
                    content = f.read().replace("@PROJECT_NAME@", project_name)

                with open(dst_file, "w") as f:
                    f.write(content)
            else:
                shutil.copyfile(src_file, dst_file)
                
            if dst_file.endswith(".sh") and os.name == 'posix':
                import stat
                st = os.stat(dst_file)
                os.chmod(dst_file, st.st_mode | stat.S_IEXEC)

def create_project(name):
    project_dir = os.path.join(DEST_ROOT, name)
    if os.path.exists(project_dir):
        print(f"Error: Project '{name}' already exists.")
        return

    os.makedirs(project_dir)
    copy_template(TEMPLATE_ROOT, project_dir, name)
    FILE_PATH = os.path.join(DEST_ROOT, "CMakeLists.txt")
    CMAKE_ADD_PROJECT = "\n" + "add_subdirectory(" + name + ")"
    with open(FILE_PATH, "a") as cmake_file:
        cmake_file.write(CMAKE_ADD_PROJECT)
    print(f"Created new project at {project_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: create_project.py <ProjectName>")
        sys.exit(1)
    create_project(sys.argv[1])