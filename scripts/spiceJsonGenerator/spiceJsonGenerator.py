# Name: SpiceJsonGenerator
# Version 1.1
# Author: Gavin Nelson
# Project: SpiceQL 

import json
import os
import fnmatch
import argparse
import warnings

''' 
kernel class object declaration will be used for storing 
all the specific kernel data associated with each kernel directory ie: kernels/fk
'''
class kernel:
    def __init__(self, kernel_type, path, mission_name):
        self.kernel_type = kernel_type 
        self.path = path
        self.mission_name = mission_name
        self.is_make_db_file = False
        self.special_key_json = None
        self.json_object = {mission_name : {}}
        self.special_json_object = {}
        

    def get_path(self):
        return self.path


    def format_kernel_file_name(self, value):
        output_filename = f""
        num_question_marks = 0
        for character in value:
            # If we see a question mark do not copy it, count it and move on
            if character == "?":
                num_question_marks += 1

            # If we have seen some question marks and now see a non-question mark
            # replace them with a regex and reset our count
            elif num_question_marks > 0:
                output_filename += "[0-9]{" + str(num_question_marks) + "}"
                num_question_marks = 0
            # Otherwise just copy the character to the output
            else:
                output_filename += character
        return output_filename

    # get's the most recent kernel to for any specific kernel type 
    def get_most_recent_kernel(self):
        make_db_version_files = []
        kernel_db_files = []  
        
        # iterate over all files in kernel dir 
        for file in os.listdir(self.path):
            # check if the file is a make db file, if so add to list
            if fnmatch.fnmatch(file, "*makedb*"):
                make_db_version_files.append(file)

            # check if the file is a kernel db file, if so add to list
            elif fnmatch.fnmatch(file, "kernels.*.db"):
                kernel_db_files.append(file)

        # check if we have make db files as make db files take priority over kernel db files 
        if make_db_version_files:
            self.is_make_db_file = True
            self.file = max(make_db_version_files)
            self.path = self.path + f"/{self.file}"

        # no make db files found, check for kernel files 
        elif kernel_db_files:
            self.file = max(kernel_db_files)
            self.path = self.path + f"/{self.file}"

        # no make db or kernel files found, throw warning 
        else:
            warnings.warn(f"\nWarning no makedb or kernel db files \
                            found for kernel type {self.kernel_type}")

    # main parse kernel call, calls respective function depending on kernel type 
    def parse_kernel(self,input_json):
        self.get_most_recent_kernel()

        if self.is_make_db_file:
            return self.parse_make_db_file(input_json)

        else: # TODO ask if I should add a self flag to be kernel file so that way we don't default 
              # to here if their are no make or kernel db files 
            return self.parse_kernel_db_file(input_json)


    # logic for parsing a make db file and pulling all of the relevant information 
    def parse_make_db_file(self,input_json):
        if not (self.kernel_type not in input_json[self.mission_name]) or not os.path.isfile(self.path):
            pass 
        
        # open file access
        file = open(self.path, "r")

        # read in all lines of file
        lines = file.readlines()
        
        # close file access 
        file.close

        #change to look for these but also default to look for 
        things_we_care_about = ["predictfilter", "reconfilter", "smithedfilter"]

        # iterate over all lines 
        for line in lines:

            # iterate over all attributes we care about 
            for item in things_we_care_about:

                # check to see if the current line has an attribute we care about 
                if item in line and not line.startswith("#"):

                    if item == "reconfilter":
                        key = self.kernel_type
                            
                        # if line.__contains__("recondir"):
                        if "recondir" in line:
                            pass

                        else:
                            # call helper 
                            self.parse_make_db_helper(key, line, input_json)


    def parse_make_db_helper(self, key, line, input_json):
        value = line.split("=")[1]
        value = value.replace('"', "")
        value = value.replace("'",'')
        value = value.strip('\(") )"\n\\ "\\ " ')
        value = value.split(",")
        # special case where value has the format of *.fileHandel 


        key_val = [self.format_kernel_file_name(string) for string in value]

        if len(value) > 1:
            value = f"{[self.format_kernel_file_name(val) for val in value]}".replace("'","")
        else:
            # join value backtogether to be a string vs a list 
            value = "".join(value)

            if value.startswith("*"):
                temp_val = value.replace("*","")
                value = f"{self.mission_name}.*\\\\{temp_val}"

            else:
                value = self.format_kernel_file_name(value)

            try:
                self.append_value(input_json[self.mission_name][self.kernel_type], "kernels", value)
            except KeyError:
                input_json[self.mission_name][self.kernel_type] = {}
                self.append_value(input_json[self.mission_name][self.kernel_type], "kernels", value)


    def get_next_line_of_kernel_file(self,lines, index):

        kernels = []
        if(self.kernel_type == "ik"):
            print(lines[44])
        while("EndGroup" not in lines[index]):
            if "File="  in lines[index].replace(" ", ""):
                line = lines[index]
                line = line.split(" ")[-1]
                line = line.strip('" ")')
                line = line.replace('")',"")
                kernel_type = line.split("/")[-2]
                value = line.split("/")[-1].strip("\n")
                kernel_pair = (kernel_type, self.format_kernel_file_name(value))
                print("here",line)
                # kernels.append()
            index += 1
        return (kernels)

    def parse_kernel_db_file(self, input_json):

        if self.kernel_type == "spk" or self.kernel_type == "cp":
            warnings.warn(f"\nWarning kernel type is {self.kernel_type} with no makedb file... Skipping and moving on")

        else:
            # variables used 
            object_start = "Object="
            object_end = "EndObject"
            special_match = "Match=("
            kernel_file = "File="
            group_end = "End_Group"
            special_case_flag = False
            special_key = ""

            if not os.path.isfile(self.path):
                return None
                    
            # open file acces
            file = open(self.path, "r")


            # read in all lines of file
            lines = file.readlines()

            # close file access 
            file.close


            # loop through lines of file 
            for index, line in enumerate(lines):

                # start of object, do nothing 
                if object_start in line.replace(" ", ""):
                    pass

                # line is a special case
                elif special_match in line.replace(" ", ""):
                    special_case_flag = True
                    special_key = line.split(",")[-1]
                    special_key = special_key.strip('\n)"')


                    # check to see if there are any dependencies 

                    # assume no dependencies 

                    
                # line is kernel file, add to mission json object
                elif kernel_file in line.replace(" ",""):

                    # check if special flag is set
                        # add to special json 
                        '''
                        ie: {
                                mission:{

                                }
                    add here --->  special:{

                                }
                            }
                        '''
                        value = line.split("/")[-1]
                        value = value.strip('\n")')
                        # print(value)
                        value = self.format_kernel_file_name(value)
                        # print(value)

                        if special_case_flag:
                            
                            kernel_type = line.split("/")[-2]
                            print("here", special_key,value)
                            try:
                                # print(value)
                                temp_json_val = {kernel_type:{"kernels":value}}

                                self.append_value(input_json, special_key, temp_json_val)
                            except KeyError:
                                input_json[special_key] = {}

                                
                            pass

                        else:
                            # print(value)
                            try:
                                
                                self.append_value(input_json[self.mission_name][self.kernel_type], "kernels", value)
                            except KeyError:
                                input_json[self.mission_name][self.kernel_type] = {}
                                self.append_value(input_json[self.mission_name][self.kernel_type], "kernels", value)
                    # assume not a special case 
                        # add to normal mission json
                        '''
                        ie: {
                                mission:{

                                    add here 
                                }
                                special:{
                                    
                                }
                            }
                        '''
                        

                
                # end of group, set special case flag to false 
                elif group_end in line.replace(" ",""):
                    pass
                
                # object end
                elif object_end  in line.replace(" ",""):
                    pass
                    # end of kernel file

    def append_value(self, dict_obj, key, value):
        # Check if key exist in dict or not
        if key in dict_obj:
            # Key exist in dict.
            # Check if type of value of key is list or not
            if not isinstance(dict_obj[key], list):
                # If type is not list then make it list
                dict_obj[key] = [dict_obj[key]]
            # Append the value in list
            dict_obj[key].append(value)
        else:
            # As key is not in dict,
            # so, add key-value pair
            dict_obj[key] = value



    # def parse_kernel_db_file(self, input_json):

        # if self.kernel_type == "spk" or self.kernel_type == "cp":
        #    warnings.warn(f"\nWarning kernel type is {self.kernel_type} with no makedb file... Skipping and moving on")

        # else:
        #     # variables used 
        #     object_start = "Object="
        #     object_end = "EndObject"
        #     special_match = "Match=("
        #     kernel_file = "File="
        #     group_end = "End_Group"
        #     special_case_flag = False

        #     if not os.path.isfile(self.path):
        #         return None
                 
        #     # open file acces
        #     file = open(self.path, "r")


        #     # read in all lines of file
        #     lines = file.readlines()

        #     # close file access 
        #     file.close


        #     # loop through lines of file 
        #     for index, line in enumerate(lines):

        #         # print(f"Here, Line=|{line}|\nline stripped =|{line.replace(" ", "")}")
        #         # start of object, do nothing 
        #         if object_start in line.replace(" ", ""):
        #             pass

        #         # line is a special case, add a new json object to end of config
                
        #         elif special_match in line.replace(" ", ""):

        #             special_case_flag = True
        #             key = line.split(",")[-1]
        #             key = key.strip('\n)"')
        #             if(self.kernel_type == "ik"):
        #                 print("yes")
        #             #     print(f"calling get next line of kernel file with the current line being:{line}, the current index being:{index}\n")
        #             kernels = self.get_next_line_of_kernel_file(lines, index + 1)
        #             kernel_deps_list = []
        #             kernel_values = []

        #             for kernel in kernels:
        #                 # check to see if kernel types match 
        #                 if kernel[0] == self.kernel_type:
        #                     # add to special cases kernel list
        #                     kernel_values.append(kernel[1])

        #                 elif kernel[0] != self.kernel_type:
        #                     # is a dep for the special cases kernel
        #                     kernel_deps_list.append(kernel)
                            
        #             # special_case_json = {} 
        #             # kernel_type_json = {} 

        #             # check to see if there are any dependencies 
        #             if len( kernel_deps_list) > 0:
        #                 kernel_deps = {}
        #                 for deps_set in kernel_deps_list:
        #                     kernel_deps["objs"] = self.mission_name + "/" + deps_set[0]
        #                     kernel_deps["kernels"] = deps_set[1]
    
        #                 kernel_type_json["deps"] = kernel_deps

        #             # assume no dependencies 
        #             # else:
        #             #     kernel_type_json["deps"] = {}

        #             # store json values 
                     
        #             if key not in self.special_json_object:
                        
        #                 # kernel_type_json["kernels"] =  kernel_values
        #                 # special_case_json = kernel_type_json
        #                 self.special_json_object[key]= {"kernels" : kernel_values}

                        
        #             # print(key)
        #             # print(json.dumps(self.special_json_object,indent=3))
                    
  
        #             # check if the key is already in the final json
        #             if key not in input_json:
        #                 pass
        #                 # input_json[key] = special_case_json

        #             elif key in input_json:
        #                 pass
                    
        #         # line is kernel file, add to mission json object 
        #         elif kernel_file in line and not special_case_flag:

        #             kernels = self.get_next_line_of_kernel_file(lines, index)
        #             kernel_values = []
        #             for kernel in kernels:
        #                 kernel_values.append((kernel[1]).replace('"',""))# left off here 

        #             value = line.split("/")[-1]
        #             value = value.strip('\n")')
        #             value = self.format_kernel_file_name(value)
        #             kernel_type_json = {} 

        #             kernel_type_json["kernels"] = kernel_values

        #             if self.kernel_type not in input_json[self.mission_name]:
        #                 input_json[self.mission_name][self.kernel_type] = kernel_type_json

        #         # end of object, set special case flag to false 
        #         elif group_end in line.replace(" ", "") and special_case_flag:
        #             special_case_flag = False

        #         # object end
        #         elif object_end in line.replace(" ", ""):
        #             pass
        #             # close file access 
        #             # print(json.dumps(self.special_json_object, indent=3))
                    
        # # print(self.path)
        # # print(json.dumps(self.special_json_object,indent=3))
                
    def get_json(self):
        return self.json_object

class mission:
    def __init__(self, path):
        self.kernel_list = []
        name = path.split("/")
        while("" in name):
            name.remove("")
        self.name = name[-1]
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
            # print(kernel.kernel_type)
            # print(json.dumps(self.json,indent=3))
            # print("\n-------\n\n")



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
        # pass
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
