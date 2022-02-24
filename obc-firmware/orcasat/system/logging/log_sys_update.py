#!/usr/bin/local/python3
from __future__ import print_function
import pickle
import os
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from jinja2 import Environment, FileSystemLoader
import datetime

"""
LOGGING SYSTEM UPDATER
    - Connects to the OBC system sheet: https://docs.google.com/spreadsheets/d/1chvy-mGuV6XV1QmsMAaXOuLLI7Z635QQu-AfypoBUl0/edit#gid=1471344190
    - Pulls out the logging IDs and generates:
        - a c header "log_ids.h"
        - a parsing function for Houston (future)

    Sheets connection based on: https://developers.google.com/sheets/api/quickstart/python

USAGE:
    - cd into orcasat/system/logging
    - python log_sys_update.py
    
    The generated files will be replaced. 
"""

# If modifying these scopes, delete the file token.pickle.
SCOPES = ['https://www.googleapis.com/auth/spreadsheets.readonly']

# ID (from sheet URL), sheet and range to pull out
SAMPLE_SPREADSHEET_ID = '1chvy-mGuV6XV1QmsMAaXOuLLI7Z635QQu-AfypoBUl0'
SAMPLE_RANGE_NAME = 'Log System IDs!A2:E'

def connect_sheets():
    """Use the Google sheets API, initialize a connection
    and grab the data from the sheet.
    """
    creds = None
    # The file token.pickle stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists('token.pickle'):
        with open('token.pickle', 'rb') as token:
            creds = pickle.load(token)

    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(
                'credentials.json', SCOPES)
            creds = flow.run_local_server()
        # Save the credentials for the next run
        with open('token.pickle', 'wb') as token:
            pickle.dump(creds, token)

    service = build('sheets', 'v4', credentials=creds)
    return service

def main():
    """Pulls out the logging system IDs from the OBC system sheet
    generates a header file with the defined IDs. 
    """
    # Connect to sheets
    sheets_connection = connect_sheets()

    # Call the Sheets API
    sheet = sheets_connection.spreadsheets()
    result = sheet.values().get(spreadsheetId = SAMPLE_SPREADSHEET_ID,
                                range = SAMPLE_RANGE_NAME).execute()
    values = result.get('values', [])

    if not values:
        print('No data found in the sheet.')
    else:
        # Get the template and feed sheets data to it
        file_loader = FileSystemLoader('./templates')
        env = Environment(loader=file_loader)
        template = env.get_template('log_ids_template.h')
        output = template.render(items=values, date = datetime.datetime.now().strftime("%I:%M %p on %B %d, %Y"))
        print(output)

        # Write the generated file
        f = open('log_ids.h', 'w+')
        f.write(output)
        f.write('\n')
        f.close()

if __name__ == '__main__':
    print("ALEASAT Logging System Updater")
    main()
