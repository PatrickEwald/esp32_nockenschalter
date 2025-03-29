document.addEventListener("DOMContentLoaded", function () {
  initWebSocket();
  loadSchedule();
  createToastContainer();
  getTimeControl();

  document
    .getElementById("scheduleForm")
    .addEventListener("submit", function (event) {
      event.preventDefault();
      saveSchedule();
      hideUnsavedChangesWarning();
    });
});

let getDataPath =
  window.location.hostname === "127.0.0.1" ? "http://localhost:3000/" : "/";

function moveServo(pos) {
  fetch(getDataPath + "moveServo?pos=" + pos)
    .then((response) => response.text())
    .then((data) => console.log(data));
}

let socket;

function initWebSocket() {
  let protocol = window.location.protocol === "https:" ? "wss://" : "ws://";
  let socketUrl = protocol + window.location.host + "/ws";

  socket = new WebSocket(socketUrl);

  socket.onopen = () => {
    console.log("✅ WebSocket verbunden");
  };

  socket.onmessage = (event) => {
    try {
      const msg = JSON.parse(event.data);
      console.log("📥 Neue Servo-Position:", msg.servo);
      updateButtonHighlight(msg.servo);
    } catch (e) {
      console.warn("❌ Ungültige WebSocket-Nachricht:", event.data);
    }
  };

  socket.onclose = () => {
    console.warn("❌ WebSocket getrennt. Versuche erneut in 3 Sekunden...");
    setTimeout(initWebSocket, 3000); // reconnect
  };
}

function updateButtonHighlight(positionName) {
  document
    .querySelectorAll("button")
    .forEach((btn) => btn.classList.remove("active"));

  const btn = document.getElementById(positionName);
  if (btn) btn.classList.add("active");
}

function loadSchedule() {
  fetch(getDataPath + "getSchedule")
    .then((response) => response.json())
    .then((data) => {
      let scheduleHTML = "";
      let timeControlEnabled = document.getElementById("toggle").checked;
      data.forEach((entry, index) => {
        let time = entry.time;
        if (time.length === 4) {
          let hours = time.substring(0, 2);
          let minutes = time.substring(2, 4);
          time = `${hours}:${minutes}`;
        }
        scheduleHTML += `
          <p>
              ${index + 1}: 
              <input type="time" id="t${
                index + 1
              }" value="${time}" placeholder="HHMM" maxlength="4" onchange="scheduleChanged()" ${
          timeControlEnabled ? "" : "disabled"
        }>
              <select id="p${index + 1}"  onchange="scheduleChanged()" ${
          timeControlEnabled ? "" : "disabled"
        }>
                  <option value="144" ${
                    entry.position === "Sauna" ? "selected" : ""
                  }>Sauna</option>
                  <option value="94" ${
                    entry.position === "Aus" ? "selected" : ""
                  }>Aus</option>
                  <option value="40" ${
                    entry.position === "Boiler" ? "selected" : ""
                  }>Boiler</option>
              </select>
          </p>`;
      });
      document.getElementById("schedule").innerHTML = scheduleHTML;
    })
    .catch((error) => console.error("Fehler beim Laden des Zeitplans:", error));
}

function saveSchedule() {
  let params = new URLSearchParams();
  for (let i = 1; i <= 4; i++) {
    let timeValue = document.getElementById("t" + i).value;

    let input = timeValue.replace(/\D/g, ""); // Nur Zahlen erlauben
    if (input.length === 4) {
      let hours = input.substring(0, 2);
      let minutes = input.substring(2, 4);
      timeValue = `${hours}${minutes}`;
    }
    let posValue = document.getElementById("p" + i).value;

    params.append("t" + i, timeValue);
    params.append("p" + i, posValue);
  }

  fetch(getDataPath + "setSchedule?" + params.toString(), { method: "GET" })
    .then((response) => response.text())
    .then((data) => {
      showToast("✅ Zeitplan gespeichert!");
      loadSchedule();
    })
    .catch((error) =>
      console.error("Fehler beim Speichern des Zeitplans:", error)
    );
}

function getTimeControl() {
  fetch(getDataPath + "getTimeControl")
    .then((response) => response.text())
    .then((data) => {
      console.log(data);
      if (data == "deaktiviert") {
        document.getElementById("toggle").checked = false;
      } else if (data == "aktiviert") {
        document.getElementById("toggle").checked = true;
      }
      loadSchedule();
    });
}

function toggleTimeControl() {
  fetch(getDataPath + "toggleTimeControl", {
    method: "GET",
  })
    .then((response) => response.text())
    .then((data) => {
      console.log(data);
      if (data == "Zeitsteuerung deaktiviert") {
        document.getElementById("toggle").checked = false;
      } else if (data == "Zeitsteuerung aktiviert") {
        document.getElementById("toggle").checked = true;
      }
      loadSchedule();
    });
}

function createToastContainer() {
  let toastContainer = document.createElement("div");
  toastContainer.id = "toast-container";
  document.body.appendChild(toastContainer);
}

function showToast(message) {
  let toast = document.createElement("div");
  toast.className = "toast";
  toast.innerText = message;
  document.getElementById("toast-container").appendChild(toast);

  setTimeout(() => {
    toast.classList.add("show");
  }, 100);

  setTimeout(() => {
    toast.classList.remove("show");
    setTimeout(() => toast.remove(), 500);
  }, 3000);
}

let isScheduleChanged = false;

function scheduleChanged() {
  if (!isScheduleChanged) {
    isScheduleChanged = true;
    showUnsavedChangesWarning();
  }
}

function showUnsavedChangesWarning() {
  let warning = document.getElementById("unsavedChangesWarning");
  if (!warning) {
    warning = document.createElement("div");
    warning.id = "unsavedChangesWarning";
    warning.innerText = "⚠ Änderungen noch nicht gespeichert!";
    document.body.appendChild(warning);
  }
}

function hideUnsavedChangesWarning() {
  let warning = document.getElementById("unsavedChangesWarning");
  if (warning) {
    isScheduleChanged = false;
    warning.remove();
  }
}
