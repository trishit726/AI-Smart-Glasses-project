import { useState } from 'react';
import { motion } from 'framer-motion';
import './App.css';

const steps = [
  { n: '01', title: 'Press the button', body: 'A physical button on the glasses frame triggers an image capture from the onboard OV2640 camera.' },
  { n: '02', title: 'Send to server',   body: 'The ESP32 posts the raw JPEG over Wi-Fi to a local Python / FastAPI server on your PC.' },
  { n: '03', title: 'GPT-4o analyses',  body: 'The server base64-encodes the image and calls OpenAI GPT-4o Vision — getting a concise scene description back in roughly a second.' },
  { n: '04', title: 'Read on OLED',     body: 'The plain-text response is returned to the glasses and rendered on a 0.96″ SSD1306 OLED display at eye level.' },
];

const stack = [
  { name: 'ESP32-S3',  sub: 'XIAO Sense MCU' },
  { name: 'OV2640',    sub: 'Camera module' },
  { name: 'GPT-4o',    sub: 'Vision model' },
  { name: 'FastAPI',   sub: 'Python backend' },
  { name: 'SSD1306',   sub: 'OLED display' },
  { name: 'Arduino',   sub: 'Firmware IDE' },
];

const captures = [
  { id: 1, ts: '22:04', imageUrl: 'https://picsum.photos/seed/coffee/900/506',    desc: 'A steaming cup of black coffee sits on a wooden desk next to a closed laptop.' },
  { id: 2, ts: '21:49', imageUrl: 'https://picsum.photos/seed/meeting/900/506',   desc: 'Three people are sitting around a conference table engaged in discussion.' },
  { id: 3, ts: '20:31', imageUrl: 'https://picsum.photos/seed/cityview/900/506',  desc: 'A wide view of a bustling city street on a sunny afternoon.' },
  { id: 4, ts: '18:12', imageUrl: 'https://picsum.photos/seed/mountain/900/506',  desc: 'Snow-capped mountain peaks surround a perfectly still alpine lake.' },
  { id: 5, ts: '16:07', imageUrl: 'https://picsum.photos/seed/nightmarket/900/506', desc: 'A neon-lit night market is alive with vendors selling colourful street food.' },
  { id: 6, ts: '14:53', imageUrl: 'https://picsum.photos/seed/cowork/900/506',    desc: 'Several laptops and screens fill a modern co-working open-plan office.' },
];

const fadeUp = (delay = 0) => ({
  initial: { opacity: 0, y: 18 },
  whileInView: { opacity: 1, y: 0 },
  viewport: { once: true },
  transition: { duration: 0.55, delay },
});

export default function App() {
  const [active, setActive] = useState(captures[0]);

  return (
    <div className="page">

      {/* ── NAV ── */}
      <nav className="nav">
        <span className="nav-logo">◈ VisionLens</span>
        <div className="nav-links">
          <a href="#how">How it works</a>
          <a href="#demo">Demo</a>
          <a href="#stack">Stack</a>
          <a href="https://github.com/trishit726/AI-Smart-Glasses-project" target="_blank" rel="noreferrer" className="nav-gh">GitHub ↗</a>
        </div>
      </nav>

      {/* ── HERO ── */}
      <section className="hero" id="hero">
        <motion.p className="hero-eyebrow" {...fadeUp(0)}>Open-source · DIY · GPT-4o</motion.p>
        <motion.h1 className="hero-h1" {...fadeUp(0.08)}>
          AI Smart<br />Glasses
        </motion.h1>
        <motion.p className="hero-body" {...fadeUp(0.16)}>
          A DIY wearable that captures your point of view, describes the scene using OpenAI's GPT-4o Vision,
          and prints the result on a tiny OLED display — all for under $20.
          Built with a Seeed XIAO ESP32-S3 and a local Python server.
        </motion.p>
        <motion.div className="hero-chips" {...fadeUp(0.22)}>
          <span className="chip">ESP32-S3</span>
          <span className="chip">OV2640 Camera</span>
          <span className="chip">GPT-4o Vision</span>
          <span className="chip">FastAPI</span>
          <span className="chip">OLED Display</span>
        </motion.div>
        <motion.div className="hero-rule" {...fadeUp(0.28)} />
      </section>

      {/* ── HOW IT WORKS ── */}
      <section className="section" id="how">
        <motion.p className="section-label" {...fadeUp(0)}>How it works</motion.p>
        <div className="steps-grid">
          {steps.map((s, i) => (
            <motion.div className="step-card" key={s.n} {...fadeUp(i * 0.07)}>
              <span className="step-n">{s.n}</span>
              <h3 className="step-title">{s.title}</h3>
              <p className="step-body">{s.body}</p>
            </motion.div>
          ))}
        </div>
      </section>

      {/* ── DEMO CAPTURES ── */}
      <section className="section" id="demo">
        <motion.p className="section-label" {...fadeUp(0)}>Live captures — demo</motion.p>
        <motion.p className="section-sub" {...fadeUp(0.06)}>
          Images below are hardcoded examples of what the glasses see. When the Python server is running locally,
          this panel refreshes automatically with real captures.
        </motion.p>

        {/* Active big view */}
        <motion.div className="demo-hero" {...fadeUp(0.1)}>
          <div className="demo-img-wrap">
            <motion.img key={active.imageUrl} src={active.imageUrl} alt=""
              initial={{ opacity: 0 }} animate={{ opacity: 1 }} transition={{ duration: 0.35 }} />
            <span className="demo-ts">{active.ts}</span>
          </div>
          <motion.p className="demo-desc" key={active.desc}
            initial={{ opacity: 0, y: 6 }} animate={{ opacity: 1, y: 0 }} transition={{ duration: 0.35, delay: 0.1 }}>
            "{active.desc}"
          </motion.p>
        </motion.div>

        {/* Thumbnail row */}
        <div className="thumb-row">
          {captures.map((c, i) => (
            <motion.button key={c.id} className={`thumb ${active.id === c.id ? 'active' : ''}`}
              onClick={() => setActive(c)}
              {...fadeUp(i * 0.05)}
              whileHover={{ scale: 1.04 }} whileTap={{ scale: 0.97 }}>
              <img src={c.imageUrl} alt="" />
            </motion.button>
          ))}
        </div>
      </section>

      {/* ── STACK ── */}
      <section className="section" id="stack">
        <motion.p className="section-label" {...fadeUp(0)}>Tech stack</motion.p>
        <div className="stack-grid">
          {stack.map((s, i) => (
            <motion.div className="stack-card" key={s.name} {...fadeUp(i * 0.06)}>
              <p className="stack-name">{s.name}</p>
              <p className="stack-sub">{s.sub}</p>
            </motion.div>
          ))}
        </div>
      </section>

      {/* ── FOOTER ── */}
      <footer className="footer">
        <p>VisionLens · Built by Trishit · <a href="https://github.com/trishit726/AI-Smart-Glasses-project" target="_blank" rel="noreferrer">GitHub ↗</a></p>
      </footer>

    </div>
  );
}
