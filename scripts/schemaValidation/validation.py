# Name: jsonValidation
# Version 1.0
# Author: Gavin Nelson
# Project: SpiceQL 

import json
import jsonschema
from jsonschema import validate
import argparse
from os.path import exists



def validate(json_path, schema_path):
    '''
    Validate json against schema

    Parameters
    ---------
    json_path: str
                path to the json file

    schema_path: str
                path the the json schema file

    Returns
    -------
    Boolean result of operation 
    '''
    if not exists(json_path) or not exists(schema_path):
        print("Bad file path")
        return False

    file = open(json_path)
    schema_file = open(schema_path)
    schema_data = json.load(schema_file)
    json_data = json.load(file)

    try:
        jsonschema.validate(instance=json_data, schema=schema_data)
    except jsonschema.exceptions.ValidationError as err:
        print(err)
        return False
    
    file.close()
    return True

def main(json_path):
    '''
    Driver for program, calles validate to validate input json file against preset schema 

    Parameters
    ----------
    json_path: str
                path to the json file

    Returns
    -------
    None
    '''
    schema_path = "../../SpiceQL/db/spiceMissionSchmea.schema.json"
    results = validate(json_path, schema_path)
    print(results)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "This is a python script to verify that any given mission json file matches a preset schema")
    parser.add_argument('json_path', help="Please enter the path to the json that you would like to validate against the schema")

    args = parser.parse_args()
    main(args.json_path)