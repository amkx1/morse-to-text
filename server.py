from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

current_morse = ""
current_message = ""


MORSE = {
    ".-": "A",
    "-...": "B",
    "-.-.": "C",
    "-..": "D",
    ".": "E",
    "..-.": "F",
    "--.": "G",
    "....": "H",
    "..": "I",
    ".---": "J",
    "-.-": "K",
    ".-..": "L",
    "--": "M",
    "-.": "N",
    "---": "O",
    ".--.": "P",
    "--.-": "Q",
    ".-.": "R",
    "...": "S",
    "-": "T",
    "..-": "U",
    "...-": "V",
    ".--": "W",
    "-..-": "X",
    "-.--": "Y",
    "--..": "Z",

    "-----": "0",
    ".----": "1",
    "..---": "2",
    "...--": "3",
    "....-": "4",
    ".....": "5",
    "-....": "6",
    "--...": "7",
    "---..": "8",
    "----.": "9"
}


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/morse", methods=["POST"])
def receive_morse():

    global current_morse
    global current_message

    data = request.get_json()
    morse = data.get("morse", "")
    message_type = data.get("type", "")

    if message_type == "letter":
        current_morse += morse + " "
        letter = MORSE.get(morse, "?")
        current_message += letter

    elif message_type == "space":
        current_morse += "    "
        current_message += " "

    print("Morse:", current_morse)
    print("Message:", current_message)

    return jsonify({
        "status": "received"
    })


@app.route("/clear", methods=["POST"])
def clear():

    global current_morse
    global current_message

    current_morse = ""
    current_message = ""

    print("Message cleared")

    return jsonify({
        "status": "cleared"
    })


@app.route("/data")
def get_data():

    return jsonify({
        "morse": current_morse,
        "message": current_message
    })


if __name__ == "__main__":

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )