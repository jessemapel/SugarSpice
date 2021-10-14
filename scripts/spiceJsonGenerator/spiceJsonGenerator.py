# Name: SpiceJsonGenerator or GGDBG 
# Version 1.0
# Author: Gavin Nelson
# Project: SpiceQL 

import json
import os
import fnmatch
import argparse
import warnings




# kernel class object declaration 
# will be used for storing all the specific kernel data associated with each kernel directory ie: kernels/fk
class kernel:
    def __init__(self, kernel_type, path, mission_name):
        self.kernel_type = kernel_type 
        self.path = path
        self.mission_name = mission_name
        self.is_make_db_file = False
        self.is_special_case = False
        self.special_key_json = None
        self.json_object = {}
        self.json_object[self.mission_name] = {}
        

    def get_path(self):
        return self.path

    def get_kernel_type(self):
        return self.kernel_type

    def get_mission_name(self):
        return self.mission_name
    
    def print_name(self):
        print(self.kernel_type)

    def format_kernel_file_name(self,value):
        test = []

        seen_a_question = False
        start_index = None
        end_index = None
        start_len = 0
        end_len = 0
        change = 0
        count = 1

        for index, character in enumerate(value):
            
            if character == "?":
                if seen_a_question == False:
                    start_index = index
                    seen_a_question = True

                elif seen_a_question:
                    count += 1

            elif character != "?" and seen_a_question:
                end_index = index   
                test.append((start_index,end_index,count))
                seen_a_question = False 
                count = 1
                start_index = None
                end_index = None

        for index, replacement in enumerate(test):

            change += start_len - end_len 

            start_len = len(value)
            value = value[:replacement[0] - change] + "[0-9]" + "{" + f"{replacement[2]}" + "}" + value[replacement[1] - change:]
            
            end_len = len(value)

        return value



    # get's the most recent kernel to for any specific kernel type 
    def get_most_recent_kernel(self):

        version_files = []  

        for file in os.listdir(self.path):
            if fnmatch.fnmatch(file, "*makedb*"):
                version_files.append(file)

        if version_files:
            self.is_make_db_file = True
            pass

        # no makedb file found result to useing kernels.xxx.db files 
        else:
            for file in os.listdir(self.path):
                version_files.append(file)

        self.file = max(version_files)
        self.path = self.path + f"/{self.file}"

    def parse_kernel(self,input_json):
        
        self.get_most_recent_kernel()


        if self.is_make_db_file:
            
            return self.parse_make_db_file(input_json)

        elif not self.is_make_db_file:
            return self.parse_kernel_db_file(input_json)
            

            
    def parse_make_db_file(self,input_json):
        # open file access
        file = open(self.path, "r")

        # read in all lines of file
        lines = file.readlines()
        

        #change to look for these but also default to look for 
        things_we_care_about = ["predictfilter", "reconfilter", "smithedfilter"]

        for line in lines:

            for item in things_we_care_about:

                if line.__contains__(item):

                    if item == "reconfilter":
                        line = line.strip(" ")
                        key = self.kernel_type

                        if self.kernel_type not in input_json[self.mission_name]:
                            
                            if line.__contains__("recondir"):
                                pass

                            else:
                                line = line.strip(" ")


                            key = self.kernel_type

                            if self.kernel_type not in input_json[self.mission_name]:
                                value = line.split("=")[1]
                                value = value.strip('\(") )"\n\\ "\\ " ')
                                value = value.replace('"', "")
                                value = value.split(",")

                                key_val = []
                                for string in value:
                                    key_val.append(self.format_kernel_file_name(string))

                                    
                                if len(value) > 1:
                                    value = f"{[self.format_kernel_file_name(val) for val in value]}".replace("'","")
                                else:
                                    value = self.format_kernel_file_name(value)

                                temp_json = {}
                                temp_json["kernels"] = key_val
                                input_json[self.mission_name][key] = {"reconstructed" : temp_json}
        file.close


    def get_next_line_of_kernel_file(self,lines, index):
        kernels = []
        while(lines[index].__contains__("File =")):
            line = lines[index]
            line = line.split(" ")[-1]
            line = line.strip('" ")')
            line = line.replace('")',"")
            kernel_type = line.split("/")[-2]
            value = line.split("/")[-1].strip("\n")
            kernels.append((kernel_type, self.format_kernel_file_name(value)))
            index += 1
        return (kernels)

    def parse_kernel_db_file(self, input_json):

        if self.kernel_type == "spk" or self.kernel_type == "cp":
           warnings.warn(f"\nWarning kernel type is {self.kernel_type} with no makedb file... Skipping and moving on")

        else:
            
            # variables used 
            object_start = "Object ="
            object_end = "EndObject"
            special_match = "Match = ("
            kernel_file = "File = "
            group_end = "End_Group"

            # open file access
            file = open(self.path, "r")

            # read in all lines of file
            lines = file.readlines()

            # loop through lines of file 
            for index, line in enumerate(lines):

                # start of object, do nothing 
                if line.__contains__(object_start):
                    pass

                # line is a special case, add a new json object to end of config
                elif line.__contains__(special_match):

                    self.is_special_case = True

                    key = line.split(",")[-1]
                    key = key.strip('\n)"')

                    kernels = self.get_next_line_of_kernel_file(lines, index + 1)

                    kernel_deps_list = []
                    kernel_values = []

                    for kernel in kernels:

                        # check to see if kernel types match 
                        if kernel[0] == self.kernel_type:
                            # add to special cases kernel list
                            kernel_values.append(kernel[1])


                        elif kernel[0] != self.kernel_type:
                            # is a dep for the special cases kernel
                            kernel_deps_list.append(kernel)
                            

                
                    special_case_json = {} 
                    kernel_type_json = {} 


                    # check to see if there are any dependencies 
                    if len( kernel_deps_list) > 0:
                        kernel_deps = {}
                        for deps_set in kernel_deps_list:
                            kernel_deps["objs"] = self.mission_name + "/" + deps_set[0]
                            kernel_deps["kernels"] = deps_set[1]
    
                        kernel_type_json["deps"] = kernel_deps

                    # assume no dependencies 
                    else:
                        kernel_type_json["deps"] = {}

                    # store json values  
                    kernel_type_json["kernels"]=  kernel_values
                    special_case_json = kernel_type_json

                        
                    # check if the key is already in the final json
                    if key not in input_json:
                        input_json[key] = special_case_json

                    elif key in input_json:
                        pass
                    

                # line is kernel file, add to mission json object 
                elif line.__contains__(kernel_file) and not self.is_special_case:

                    kernels = self.get_next_line_of_kernel_file(lines, index)
                    kernel_values = []
                    for kernel in kernels:
                        kernel_values.append((kernel[1]).replace('"',""))# left off here 

                    value = line.split("/")[-1]
                    value = value.strip('\n")')
                    value = self.format_kernel_file_name(value)
                    kernel_type_json = {} 

                    kernel_type_json["kernels"] = kernel_values

                    if self.kernel_type not in input_json[self.mission_name]:
                        input_json[self.mission_name][self.kernel_type] = kernel_type_json

                # end of object, set special case flag to false 
                elif line.__contains__(group_end) and self.is_special_case:
                    self.is_special_case = False

                # object end
                elif line.__contains__(object_end):
                    # close file access 
                    file.close
                

    def get_json(self):
        return self.json_object

 


class mission:
    def __init__(self, path):
        self.kernel_list = []
        self.name = path.split("/")[-1]
        self.path = path
        self.json = {}


    def add_kernel_to_list(self, kernel):
        self.kernel_list.append(kernel)
    
    def get_kernels(self):
        for kernel_type in os.listdir(self.path+"/kernels"):
            new_kernel = kernel(kernel_type, self.path + "/kernels/" + kernel_type, self.name)
            self.add_kernel_to_list(new_kernel)
    
    
    def generate_mission_json_config(self):
        self.json[self.name] = {}

        for kernel in self.kernel_list:
            kernel.parse_kernel(self.json)






# arg parse (basic help stuff too)
# make scripts folder, and env .yml that has deps for running 


def main(mission_path, log_to_code):
    spice_home_dir = os.path.abspath(os.path.join(__file__,"../../../"))
    new_mission = mission(mission_path)
    new_mission.get_kernels()
    new_mission.generate_mission_json_config()


    if log_to_code.lower() == "file":
        file = open(spice_home_dir + f"/SpiceQL/db/{new_mission.name}.json", "w+")
        json.dump(new_mission.json,file, indent=4)
        print("to file")
        file.close()

    elif log_to_code.lower() == "terminal":
        print(json.dumps(new_mission.json,indent=4))
    
    elif log_to_code.lower() == "both":
        file = open(spice_home_dir + f"/SpiceQL/db/{new_mission.name}.json", "w+")
        json.dump(new_mission.json,file, indent=4)
        print("to file")
        file.close()
        print(json.dumps(new_mission.json,indent=4))




if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = "A script to automatically generate mission json files the file will be placed in the SpiceQL/db folder\n\
                                                    Note: there are some weird edge cases that this script \
                                                    does not catch yet, specifically when it come's to kernels having\n\
                                                    special dependency object's that are a different kernel type\
                                                    \n\nTo run this script you must enter in a mission file path and a log code\n\
                                                    ie: python spiceJsonGenerator.py <folder path> <log code>")
    parser.add_argument('mission_path', help="\n\
                                        enter the path the mission folder \n\
                                        ie: /Users/bob/scratch/isis_data/cassini\n")

    parser.add_argument('log_to_code', help="\nJson loggin destination\nComand options\n\
                                            File: log's output to a json file\n\
                                            Terminal: logs output to terminal\n\
                                            Both: logs output to both terminal and json file")
    args = parser.parse_args()
    main(args.mission_path, args.log_to_code)
