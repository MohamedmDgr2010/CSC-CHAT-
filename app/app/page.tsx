'use client';
import {useEffact,useState} from 'react'
import Image from "next/image";
let jwt="";

const sock=new WebSocket("ws://localhost:2024");
function Header(){
  return(
    <header>
      <nav>
        <h1>CSC CHAT </h1>
        
      </nav>
    </header>);
}
async function req(url, m = 'GET', body = null) {
  const options = {
    method: m,
    headers: {
      "Content-Type": "application/json",
      "jwt": jwt
    }
  };

  if (body) {
    options.body = JSON.stringify(body);
  }

  const res = await fetch(url, options);

  if (!res.ok) {
    throw new Error(`خطأ في التحميل: ${res.status}`);
  }

  return await res.json();
}
let Chat=ChatBox();
function Win(){
  
  return(<main>
    <Menu/>
    {Chat}
  </main>);
}
function ChatBox(w=""){
  if(w=="")return(<div>Select Chat </div>);
  return(
    <div>
     <div>
        <button onClick={() => router.push(`/user?username=${w.username}`)}>
            {w.name}
        </button>
      </div>
    <div>
        
        
    </div>
      
      <div className="Input">
        <input id="msgIn" />

        <button onClick={() => {
  const msg = {
    payload: document.getElementById("msgIn").value,
    to: w.username
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
function Menu(){
  const chats=await get("https://localhost:1024/mychats");
  if(!Array.isArray(chats))throw now Error "Failed to load chats";
  
  //{"chat":chatUrl,name,msgs:{from,payload}}
  return (
  <div>
    {chats.map((c, i) => (

        <button onClick={() => chaingeChat(c.chat)}>
          {c.name}
        </button>
      
    ))}
  </div>
);
  
}
export default function Home() {
  return(
    <Header/>
    <Win/>
    );
}

