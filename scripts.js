document.addEventListener('DOMContentLoaded', function() {
    const logDiv = document.getElementById('log');
    const outputDiv = document.getElementById('output');
    const startButton = document.getElementById('start-recognition');
    const manualInputSection = document.getElementById('manual-input-section');
    const manualInput = document.getElementById('manual-input');
    let ip;

    // Function to send data to ESP8266
    function turnDevice(device, status) {
        var xhr = new XMLHttpRequest();
        xhr.open("POST", `http://192.168.${ip}/update`, true);
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.send(`device=${device}&status=${status}`);
    }

    // Function to log messages
    function logMessage(message) {
        console.log(message);
        const newLogEntry = document.createElement('p');
        newLogEntry.textContent = message;
        logDiv.appendChild(newLogEntry);
    }

    // Function to start speech recognition with duration
    function startRecognition(recognition, duration) {
        recognition.start();
        logMessage('Speech recognition started.');

        setTimeout(() => {
            recognition.stop();
            logMessage(`Speech recognition stopped after ${duration / 1000} seconds.`);
        }, duration);
    }

    // Set up speech recognition
    function setupSpeechRecognition() {
        if (annyang) {
            logMessage('Annyang is loaded and ready.');
            const commands = {
                '*term': function(term) {
                    outputDiv.textContent = term;
                    logMessage('Speech recognized: ' + term);
                    handleCommand(term);
                }
            };

            annyang.addCommands(commands);
            startButton.addEventListener('click', function() {
                try {
                    annyang.start({ autoRestart: false, continuous: false });
                    logMessage('Speech recognition started.');
                
                    setTimeout(function() {
                        annyang.abort();
                        logMessage('Speech recognition stopped after 10 seconds.');
                    }, 10000);
                } catch (error) {
                    logMessage('Error starting speech recognition: ' + error.message);
                }
            });
        } else if ('SpeechRecognition' in window || 'webkitSpeechRecognition' in window) {
            const recognition = new (window.SpeechRecognition || window.webkitSpeechRecognition)();
            recognition.lang = 'en-US';
            recognition.interimResults = false;

            recognition.onresult = function(event) {
                const transcript = event.results[0][0].transcript;
                outputDiv.textContent = transcript;
                logMessage('Speech recognized: ' + transcript);
                handleCommand(transcript);
            };

            recognition.onerror = function(event) {
                logMessage('Speech recognition error detected: ' + event.error);
                alert('Speech recognition error: ' + event.error);
            };

            startButton.addEventListener('click', function() {
                startRecognition(recognition, 10000);
            });
        } else {
            manualInputSection.style.display = 'block';
            logMessage('Speech recognition not supported by your browser.');
            alert('Speech recognition not supported by your browser.');

            manualInput.addEventListener('input', function() {
                const value = this.value;
                outputDiv.textContent = value;
                logMessage('Manual input: ' + value);
                handleCommand(value);
            });
        }
    }

    // Function to handle recognized commands
    function handleCommand(term) {
        term = term.toLowerCase();
        if (term.includes("turn on led") && !term.includes("do not") && !term.includes("don't")) {
            turnDevice('led', 'on');
        } else if (term.includes("turn off led") && !term.includes("do not") && !term.includes("don't")) {
            turnDevice('led', 'off');
        } else if (term.includes("turn on fan") && !term.includes("do not") && !term.includes("don't")) {
            turnDevice('fan', 'on');
        } else if (term.includes("turn off fan") && !term.includes("do not") && !term.includes("don't")) {
            turnDevice('fan', 'off');
        } else if (term.includes("do not turn on led") || term.includes("don't turn on led")) {
            logMessage('Command ignored: Do not turn on LED');
        } else if (term.includes("do not turn off led") || term.includes("don't turn off led")) {
            logMessage('Command ignored: Do not turn off LED');
        } else if (term.includes("do not turn on fan") || term.includes("don't turn on fan")) {
            logMessage('Command ignored: Do not turn on fan');
        } else if (term.includes("do not turn off fan") || term.includes("don't turn off fan")) {
            logMessage('Command ignored: Do not turn off fan');
        } else {
            logMessage('No valid command found.');
        }
    }

    // Set IP address on submit
    document.getElementById('submitButton').addEventListener('click', function() {
        ip = document.getElementById('ipaddress').value;
        alert(`IP Address set to: 192.168.${ip}`);
    });

    document.getElementById('onbtn').addEventListener('click', function() {
        turnDevice("led", "on");
    });
    document.getElementById('offbtn').addEventListener('click', function() {
        turnDevice("led", "off");
    });
        document.getElementById('onbtnFan').addEventListener('click', function() {
            turnDevice("fan", "on");
        });
        document.getElementById('offbtnFan').addEventListener('click', function() {
            turnDevice("fan", "off");
        });
    

    // Initialize speech recognition
    setupSpeechRecognition();
});
