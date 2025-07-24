import './App.css';
import { useEffect, useState } from 'react';

let jwt = "";
const sock = new WebSocket("ws://localhost:2024");

function Header() {
  return (
    <header>
      <nav>
        <h1>CSC CHAT</h1>
      </nav>
    </header>
  );
}

async function req(url, m = 'GET', body = null) {
  const options = {
    method: m,
    headers: {
      "Content-Type": "application/json",
      "jwt": jwt
    }
  };

  if (body) options.body = JSON.stringify(body);

  const res = await fetch(url, options);
  if (!res.ok) throw new Error(`خطأ في التحميل: ${res.status}`);

  return await res.json();
}

function ChatBox({ user }) {
 if (!user) return <div>Select Chat</div>;

  return (
    <div>
      <div>
        <button onClick={() => console.log("انتقال إلى", user.username)}>
          {user.name}
        </button>
      </div>

      <div className="Input">
        <input id="msgIn" />
        <button onClick={() => {
          const msg = {
            payload: document.getElementById("msgIn").value,
            to: user.username
          };
          if (sock.readyState === WebSocket.OPEN) {
            sock.send(JSON.stringify(msg));
          } else {
            console.error("WebSocket غير متصل");
          }
        }}>
          Send
        </button>
      </div>
    </div>
  );
}

function Win() {
  const [chats, setChats] = useState([]);
  const [selectedChat, setSelectedChat] = useState(null);

  useEffect(() => {
    req("https://localhost:1024/mychats")
      .then(data => {
        if (Array.isArray(data)) setChats(data);
        else throw new Error("Invalid chat list");
      })
      .catch(console.error);
  }, []);

  return (
    <main>
      <div>
        {chats.map((c, i) => (
          <button key={i} onClick={() => setSelectedChat(c)}>
            {c.name}
          </button>
        ))}
      </div>
      <ChatBox user={selectedChat} />
    </main>
  );
}

export default function Home() {
  return (
    <>
      <Header />
      <Win />
    </>
  );
}