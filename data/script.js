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

const getDataPath =
  window.location.hostname === "127.0.0.1" ? "http://localhost:3000/" : "/";

async function moveServo(pos) {
  try {
    const response = await fetch(`${getDataPath}moveServo?pos=${pos}`);
    const data = await response.text();
    console.log(data);
  } catch (error) {
    console.error("Fehler beim Bewegen des Servos:", error);
  }
}

let socket;

function initWebSocket() {
  const protocol = window.location.protocol === "https:" ? "wss://" : "ws://";
  const socketUrl = `${protocol}${window.location.host}/ws`;

  socket = new WebSocket(socketUrl);

  socket.onopen = () => {
    console.log("✅ WebSocket verbunden");
  };

  socket.onmessage = (event) => {
    try {
      const msg = JSON.parse(event.data);
      if (msg.time) {
        updateTimeDisplay(msg.time);
      }
      if (msg.servo) {
        updateButtonHighlight(msg.servo);
        console.log("📥 Neue Servo-Position:", msg.servo);
      }
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

async function loadSchedule() {
  try {
    const response = await fetch(`${getDataPath}getSchedule`);
    const data = await response.json();
    let scheduleHTML = "";
    const timeControlEnabled = document.getElementById("toggle").checked;
    data.forEach((entry, index) => {
      let time = entry.time;
      if (time.length === 4) {
        const hours = time.substring(0, 2);
        const minutes = time.substring(2, 4);
        time = `${hours}:${minutes}`;
      }
      scheduleHTML += `
        <p>
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
  } catch (error) {
    console.error("Fehler beim Laden des Zeitplans:", error);
  }
}

async function saveSchedule() {
  const params = new URLSearchParams();
  for (let i = 1; i <= 4; i++) {
    let timeValue = document.getElementById("t" + i).value;

    const input = timeValue.replace(/\D/g, ""); // Nur Zahlen erlauben
    if (input.length === 4) {
      const hours = input.substring(0, 2);
      const minutes = input.substring(2, 4);
      timeValue = `${hours}${minutes}`;
    }
    const posValue = document.getElementById("p" + i).value;

    params.append("t" + i, timeValue);
    params.append("p" + i, posValue);
  }

  try {
    const response = await fetch(
      `${getDataPath}setSchedule?${params.toString()}`,
      { method: "GET" }
    );
    const data = await response.text();
    showToast("✅ Zeitplan gespeichert!");
    toggleSaveButton();
    loadSchedule();
  } catch (error) {
    console.error("Fehler beim Speichern des Zeitplans:", error);
  }
}

async function getTimeControl() {
  try {
    const response = await fetch(`${getDataPath}getTimeControl`);
    const data = await response.text();
    if (data == "deaktiviert") {
      document.getElementById("toggle").checked = false;
    } else if (data == "aktiviert") {
      document.getElementById("toggle").checked = true;
    }
    loadSchedule();
  } catch (error) {
    console.error("Fehler beim Abrufen der Zeitsteuerung:", error);
  }
}

async function toggleTimeControl() {
  try {
    const response = await fetch(`${getDataPath}toggleTimeControl`, {
      method: "GET",
    });
    const data = await response.text();
    if (data == "Zeitsteuerung deaktiviert") {
      document.getElementById("toggle").checked = false;
    } else if (data == "Zeitsteuerung aktiviert") {
      document.getElementById("toggle").checked = true;
    }
    loadSchedule();
  } catch (error) {
    console.error("Fehler beim Umschalten der Zeitsteuerung:", error);
  }
}

function createToastContainer() {
  const toastContainer = document.createElement("div");
  toastContainer.id = "toast-container";
  document.body.appendChild(toastContainer);
}

function showToast(message) {
  const toast = document.createElement("div");
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

function toggleSaveButton() {
  const saveButton = document.querySelector("input[type=submit]");

  if (isScheduleChanged) {
    saveButton.disabled = false;
  } else {
    saveButton.disabled = true;
  }
}

function scheduleChanged() {
  if (!isScheduleChanged) {
    isScheduleChanged = true;
    toggleSaveButton();
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
  const warning = document.getElementById("unsavedChangesWarning");
  if (warning) {
    isScheduleChanged = false;
    warning.remove();
  }
}

function updateTimeDisplay(time) {
  const timeElement = document.getElementById("currentTime");
  if (timeElement) {
    timeElement.textContent = time;
  }
}
