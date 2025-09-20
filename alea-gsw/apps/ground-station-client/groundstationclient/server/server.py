from apscheduler.jobstores.base import JobLookupError
from flask import Flask, request
from openapi_core import OpenAPI
from openapi_core.contrib.flask.decorators import FlaskOpenAPIViewDecorator

from groundstationclient.scheduler.tasks import add_contact, get_contact, get_contacts, \
    remove_contact

openapi = OpenAPI.from_file_path('./api/openapi.json')
openapi_validated = FlaskOpenAPIViewDecorator(
    openapi,
    # response_cls=None # response validation, uncomment to skip
)

app = Flask(__name__)


@app.route('/contacts', methods=['GET'])
@openapi_validated
def contacts_get():
    """Get all contacts"""

    return (
        [get_contacts()],
        200,
        {
            'Content-Type': 'application/json'
        },
    )


@app.route('/contacts', methods=['POST'])
@openapi_validated
def contacts_post():
    """Add a new contact"""

    data = request.json

    try:
        contact = get_contact(data['id'])
        return (
            {
                'code': 400,
                'message': 'Contact already exists'
            },
            400,
            {
                'Content-Type': 'application/json'
            },
        )
    except JobLookupError:
        contact = add_contact(data)
        return contact, 200, {'Content-Type': 'application/json'}


@app.route('/contacts/<string:contact_id>', methods=['GET'])
@openapi_validated
def contacts_get_by_id(contact_id):
    """Get a specific contact if it exists"""

    try:
        contact = get_contact(contact_id)
        return contact, 200, {'Content-Type': 'application/json'}
    except JobLookupError:
        return (
            {
                'code': 400,
                'message': 'Contact does not exist'
            },
            400,
            {
                'Content-Type': 'application/json'
            },
        )


@app.route('/contacts/<string:contact_id>', methods=['DELETE'])
@openapi_validated
def contacts_delete(contact_id):
    """Delete a specific contact if it exists"""

    try:
        remove_contact(contact_id)
    except JobLookupError:
        return (
            {
                'code': 400,
                'message': 'Contact does not exist'
            },
            400,
            {
                'Content-Type': 'application/json'
            },
        )

    return 'OK', 200, {'Content-Type': 'text/plain'}


def serve():
    app.run(debug=True, port=3000)
