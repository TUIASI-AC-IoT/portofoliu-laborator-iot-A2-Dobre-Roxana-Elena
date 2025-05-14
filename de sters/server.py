from flask import Flask, render_template_string, request, jsonify

app = Flask(__name__)

@app.route("/")
def index():
    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Teste API</title>
        <script>
            function callAPI(method) {
                fetch('/api/test', {
                    method: method,
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: method !== 'GET' ? JSON.stringify({data: "exemplu"}) : null
                })
                .then(response => response.json())
                .then(data => alert("Raspuns de la " + method + ": " + data.msg))
                .catch(err => alert("Eroare: " + err));
            }
        </script>
    </head>
    <body>
        <h1>First try</h1>
        <button onclick="callAPI('GET')">GET API</button>
        <button onclick="callAPI('POST')">POST API</button>
        <button onclick="callAPI('PUT')">PUT API</button>
    </body>
    </html>
    '''
    return render_template_string(html)

@app.route("/api/test", methods=["GET", "POST", "PUT"])
def api_test():
    if request.method == "GET":
        return jsonify({"msg": "GET primit!"})
    elif request.method == "POST":
        data = request.get_json()
        return jsonify({"msg": f"POST primit cu data: {data}"})
    elif request.method == "PUT":
        data = request.get_json()
        return jsonify({"msg": f"PUT primit cu data: {data}"})

if __name__ == '__main__':
    app.run(debug=True)
