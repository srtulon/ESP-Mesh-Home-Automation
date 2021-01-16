import requests


def test_request_response():
    # Send a request to the API server and store the response.
    response = requests.get('https://github.com/srtulon/ESP-Mesh-Home-Automation')

    # Confirm that the request-response cycle completed successfully.
    print(response.text)
