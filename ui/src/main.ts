import { SerialLink, type Packet } from "./serial";

const app = document.querySelector<HTMLDivElement>("#app")!;
app.innerHTML = `
  <div style="font-family: system-ui; padding: 16px; max-width: 980px; margin: 0 auto;">
    <h2 style="margin: 0 0 12px 0;">Analog Synth Control + Telemetry View<br>(Not working yet)</h2>

    <div style="display:flex; gap:12px; flex-wrap:wrap; margin-bottom:12px;">
      <button id="connect">Connect</button>
      <button id="disconnect" disabled>Disconnect</button>
      <span id="status" style="padding:4px 6px; border:1px solid #df4343ff; border-radius:8px; font-size:12px;">Disconnected</span>
    </div>

    <div style="display:grid; grid-template-columns: 1fr 1fr; gap: 12px;">
      <div style="border:1px solid #ddd; border-radius: 12px; padding: 12px;">
        <h3 style="margin-top:0;">Controls (to add to later)</h3>

        <label>Cutoff
          <input id="cutoff" type="range" min="0" max="1" step="0.001" value="0.5" style="width:100%;" />
        </label>
        <div><small>Value: <span id="cutoffVal">0.500</span></small></div>

        <label style="display:block; margin-top:10px;">Resonance
          <input id="res" type="range" min="0" max="1" step="0.001" value="0.2" style="width:100%;" />
        </label>
        <div><small>Value: <span id="resVal">0.200</span></small></div>

        <button id="sendTest" style="margin-top:12px;">Send Test Packet</button>
        <p style="color:#666; margin-bottom:0;">
          These send packets like: <code>SET &lt;paramId&gt; &lt;value&gt;</code>
        </p>
      </div>

      <div style="border:1px solid #ddd; border-radius: 12px; padding: 12px;">
        <h3 style="margin-top:0;">Live Telemetry</h3>
        <div style="display:flex; gap:10px; flex-wrap:wrap; margin-bottom:10px;">
          <div>Envelope: <b id="env">—</b></div>
          <div>RMS: <b id="rms">—</b></div>
          <div>Peak: <b id="peak">—</b></div>
        </div>
        <textarea id="log" readonly style="width:100%; height: 260px; font-family: ui-monospace, SFMono-Regular; font-size: 12px;"></textarea>
      </div>
    </div>
    <p>This is all just a template to work on later</p>
  </div>`;

const btnConnect = document.querySelector<HTMLButtonElement>("#connect")!;
const btnDisconnect = document.querySelector<HTMLButtonElement>("#disconnect")!;
const statusEl = document.querySelector<HTMLSpanElement>("#status")!;
const logEl = document.querySelector<HTMLTextAreaElement>("#log")!;

const cutoff = document.querySelector<HTMLInputElement>("#cutoff")!;
const cutoffVal = document.querySelector<HTMLSpanElement>("#cutoffVal")!;
const res = document.querySelector<HTMLInputElement>("#res")!;
const resVal = document.querySelector<HTMLSpanElement>("#resVal")!;
const envEl = document.querySelector<HTMLElement>("#env")!;
const rmsEl = document.querySelector<HTMLElement>("#rms")!;
const peakEl = document.querySelector<HTMLElement>("#peak")!;
const sendTest = document.querySelector<HTMLButtonElement>("#sendTest")!;

function log(line: string) {
  logEl.value += line + "\n";
  logEl.scrollTop = logEl.scrollHeight;
}

const link = new SerialLink({
  onStatus: (s) => {
    statusEl.textContent = s;
    const connected = s.startsWith("Connected");
    btnDisconnect.disabled = !connected;
  },
  onPacket: (pkt) => handlePacket(pkt),
  onLine: (line) => log(line),
});

btnConnect.onclick = async () => {
  try {
    await link.connect();
    log("[ui] connected");
  } catch (e) {
    log(`[ui] connect error: ${String(e)}`);
  }
};

btnDisconnect.onclick = async () => {
  await link.disconnect();
  log("[ui] disconnected");
};

function sendParam(paramId: number, value01: number) {
  // SET <id> <value>
  // value stays 0 to 1 for UI; firmware maps to actual CV.
  link.sendLine(`SET ${paramId} ${value01.toFixed(4)}`);
}

cutoff.oninput = () => {
  cutoffVal.textContent = Number(cutoff.value).toFixed(3);
  // ex 10=cutoff, 11=res, ...
  sendParam(10, Number(cutoff.value));
};

res.oninput = () => {
  resVal.textContent = Number(res.value).toFixed(3);
  sendParam(11, Number(res.value));
};

sendTest.onclick = () => {
  link.sendLine("PING");
};

function handlePacket(pkt: Packet) {
  if (pkt.type === "telemetry") {
    envEl.textContent = pkt.env.toFixed(3);
    rmsEl.textContent = pkt.rms.toFixed(3);
    peakEl.textContent = pkt.peak.toFixed(3);
  }
}
