#include <WiFiS3.h>       // Libreria per la gestione del WiFi su Arduino Uno R4 Wi-Fi
#include <WiFiServer.h>   // Libreria per avviare un server web
#include "arduino_secrets.h" // File che contiene credenziali WiFi (SSID e password)
//#include "cert.h"
//#include "key.h"

// Dichiarazione delle credenziali WiFi lette dal file "arduino_secrets.h"
char ssid[] = SECRET_SSID;  
char pass[] = SECRET_PASS;  
int status = WL_IDLE_STATUS; // Stato iniziale della connessione WiFi

const int relay1Pin = 2;
const int relay2Pin = 3;
const int relay3Pin = 4;
const int relay4Pin = 5;
const int relay5Pin = 6;
const int relay6Pin = 7;
const int relay7Pin = 8;
const int relay8Pin = 9;

// Variabili per il login
String correctUsername = "admin";  // Nome utente
String correctPassword = "IonoSpNPS";  // Password
bool loggedIn = false;  // Variabile per tracciare se l'utente è loggato


// Creazione di un server web sulla porta 80
WiFiServer server(80); 

void setup() {
    Serial.begin(9600); // Inizializzazione della comunicazione seriale a 9600 baud
    while (!Serial);    // Attende l'apertura della porta seriale

    // Inizializzazione dei pin dei relè
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
  pinMode(relay5Pin, OUTPUT);
  pinMode(relay6Pin, OUTPUT);
  pinMode(relay7Pin, OUTPUT);
  pinMode(relay8Pin, OUTPUT);
  

  // Impostazione dello stato iniziale dei relè
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay3Pin, HIGH);
  digitalWrite(relay4Pin, HIGH);
  digitalWrite(relay5Pin, LOW);
  digitalWrite(relay6Pin, LOW);
  digitalWrite(relay7Pin, LOW);
  digitalWrite(relay8Pin, LOW);
  

    // Controlla se il modulo WiFi è presente
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!"); // Errore critico
        while (true); // Blocca l'esecuzione
    }

    // Connessione alla rete WiFi
    while (status != WL_CONNECTED) {
        Serial.print("Connecting to: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass); // Tenta la connessione
        delay(10000); // Attende 10 secondi prima di ritentare
    }

    // Connessione avvenuta con successo
    Serial.println("Connected to WiFi");
    server.begin(); // Avvia il server web
    Serial.println("Web server started");
    Serial.print("Indirizzo IP: ");
    Serial.println(WiFi.localIP()); // Mostra l'IP assegnato alla scheda
}

// Funzione per restituire lo stato del relè (verde o rosso) in base a ciascun relè
String getRelayStatus(int relayPin) {
  if (relayPin == relay1Pin) {
    return digitalRead(relay1Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay2Pin) {
    return digitalRead(relay2Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay3Pin) {
    return digitalRead(relay3Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay4Pin) {
    return digitalRead(relay4Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay5Pin) {
    return digitalRead(relay5Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay6Pin) {
    return digitalRead(relay6Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay7Pin) {
    return digitalRead(relay7Pin) == HIGH ? "on-status" : "off-status";
  } else if (relayPin == relay8Pin) {
    return digitalRead(relay8Pin) == HIGH ? "on-status" : "off-status";
  } 
  
  return "off-status"; // Default caso, in caso non venga trovato nessun relè valido
}

// Funzione per la pagina di login
String getLoginPage() {
  String page = "<html><head>";
  page += "<style>";
  page += "body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }";
  page += "h2 { margin-bottom: 20px; }";
  page += "input[type='text'], input[type='password'] {";
  page += "  padding: 10px;";
  page += "  margin: 10px 0;";
  page += "  width: 200px;";
  page += "  font-size: 16px;";
  page += "}";
  page += "input[type='submit'] {";
  page += "  padding: 10px 20px;";
  page += "  font-size: 16px;";
  page += "  cursor: pointer;";
  page += "  background-color: #4CAF50;";
  page += "  color: white;";
  page += "  border: none;";
  page += "  border-radius: 5px;";
  page += "  margin-top: 10px;";
  page += "}";
  page += "input[type='submit']:hover {";
  page += "  background-color: #45a049;";
  page += "}";
  page += "</style>";
  page += "</head><body>";

  page += "<h2>NPS INGV LOGIN</h2>";
  page += "<form method='POST' action='/login'>";
  page += "Username: <input type='text' name='username'><br>";
  page += "Password: <input type='password' name='password'><br>";
  page += "<input type='submit' value='Login'>";
  page += "</form>";

  page += "</body></html>";
  return page;
}


// Funzione per estrarre il valore dal corpo della richiesta POST
String getValue(String data, String key) {
  int start = data.indexOf(key + "=");
  if (start == -1) return ""; // Se il parametro non esiste

  start += key.length() + 1; // Salta la chiave e il simbolo "="
  int end = data.indexOf("&", start);
  if (end == -1) end = data.length(); // Fino alla fine della stringa

  return data.substring(start, end);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;  // Se non ci sono richieste, esci dalla funzione
  }

  // Legge la richiesta inviata dal client
  String request = client.readStringUntil('\r');
  client.flush();

  // Gestione login (richiesta POST per il login)
  if (request.indexOf("/login") != -1 && request.indexOf("POST") != -1) {
    // A questo punto dobbiamo leggere i dati POST nel corpo della richiesta
    String postData = "";
    while (client.available()) {
      postData += (char)client.read();
    }

    // Una volta che abbiamo i dati POST, possiamo analizzarli
    String username = getValue(postData, "username");
    String password = getValue(postData, "password");

    if (username == correctUsername && password == correctPassword) {
      loggedIn = true;
    }

    // Reindirizza alla pagina principale
    client.println("HTTP/1.1 303 See Other");
    client.println("Location: /");
    client.println();
    client.stop();
    return;
  }

  // Gestione della richiesta di logout
  if (request.indexOf("/logout") != -1) {
    loggedIn = false;
    client.println("HTTP/1.1 303 See Other");
    client.println("Location: /");
    client.println();
    client.stop();
    return;
  }

  // Se non loggato, reindirizza alla pagina di login
  if (!loggedIn && request.indexOf("/") != -1) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.print(getLoginPage());
    client.stop();
    return;
  }

  // Se l'utente è loggato, procedi con il controllo dei relè
  if (loggedIn) {
    // Gestione delle richieste per ogni relè
    if (request.indexOf("/relay1/on") != -1) {
      digitalWrite(relay1Pin, HIGH);
    } else if (request.indexOf("/relay1/off") != -1) {
      digitalWrite(relay1Pin, LOW);
    }

    if (request.indexOf("/relay2/on") != -1) {
      digitalWrite(relay2Pin, HIGH);
    } else if (request.indexOf("/relay2/off") != -1) {
      digitalWrite(relay2Pin, LOW);
    }

    if (request.indexOf("/relay3/on") != -1) {
      digitalWrite(relay3Pin, HIGH);
    } else if (request.indexOf("/relay3/off") != -1) {
      digitalWrite(relay3Pin, LOW);
    }

    if (request.indexOf("/relay4/on") != -1) {
      digitalWrite(relay4Pin, HIGH);
    } else if (request.indexOf("/relay4/off") != -1) {
      digitalWrite(relay4Pin, LOW);
    }

    if (request.indexOf("/relay5/on") != -1) {
      digitalWrite(relay5Pin, HIGH);
    } else if (request.indexOf("/relay5/off") != -1) {
      digitalWrite(relay5Pin, LOW);
    }

    if (request.indexOf("/relay6/on") != -1) {
      digitalWrite(relay6Pin, HIGH);
    } else if (request.indexOf("/relay6/off") != -1) {
      digitalWrite(relay6Pin, LOW);
    }

    if (request.indexOf("/relay7/on") != -1) {
      digitalWrite(relay7Pin, HIGH);
    } else if (request.indexOf("/relay7/off") != -1) {
      digitalWrite(relay7Pin, LOW);
    }

    if (request.indexOf("/relay8/on") != -1) {
      digitalWrite(relay8Pin, HIGH);
    } else if (request.indexOf("/relay8/off") != -1) {
      digitalWrite(relay8Pin, LOW);
    }

    // Creazione della risposta HTML per i relè
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>INGV NPS Controllo Dispositivi</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; text-align: center; }";
    html += ".btn { padding: 10px 20px; margin: 5px; font-size: 18px; border: none; cursor: pointer; }";
    html += ".on { background-color: green; color: white; }";
    html += ".off { background-color: red; color: white; }";
    html += ".status { width: 15px; height: 15px; border-radius: 50%; display: inline-block; margin-right: 10px; border: 2px solid black; }"; // Per il pallino
    html += ".on-status { background-color: green; }"; // Verde quando acceso
    html += ".off-status { background-color: red; }"; // Rosso quando spento
    html += "</style>";
    html += "</head><body>";
    html += "<h1>   Controllo Dispositivi   </h1>";

    // Aggiorna lo stato di ciascun relè e aggiungi i pulsanti
    html += "<p>SP044 <span class='status " + getRelayStatus(relay1Pin) + "'></span>";
    html += "<a href='/relay1/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay1/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>AMPLI <span class='status " + getRelayStatus(relay2Pin) + "'></span>";
    html += "<a href='/relay2/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay2/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>LINUX <span class='status " + getRelayStatus(relay3Pin) + "'></span>";
    html += "<a href='/relay3/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay3/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>PC-WI <span class='status " + getRelayStatus(relay4Pin) + "'></span>";
    html += "<a href='/relay4/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay4/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>Relè 5 <span class='status " + getRelayStatus(relay5Pin) + "'></span>";
    html += "<a href='/relay5/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay5/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>Relè 6 <span class='status " + getRelayStatus(relay6Pin) + "'></span>";
    html += "<a href='/relay6/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay6/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>Relè 7 <span class='status " + getRelayStatus(relay7Pin) + "'></span>";
    html += "<a href='/relay7/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay7/off'><button class='btn off'>Spegni</button></a></p>";

    html += "<p>Relè 8 <span class='status " + getRelayStatus(relay8Pin) + "'></span>";
    html += "<a href='/relay8/on'><button class='btn on'>Accendi</button></a>";
    html += "<a href='/relay8/off'><button class='btn off'>Spegni</button></a></p>";

    html += "</body></html>";

    // Invia la risposta HTML al client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(html);
    client.stop();
  }
}
