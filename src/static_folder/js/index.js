const img_container = document.body.appendChild(document.createElement("div"));

for (let i = 0; i < 5; i++) {

	img_container.id = "img_container";
	const ele = img_container.appendChild(document.createElement("div"));
	ele.className = "cards"
}

const socket = new WebSocket("ws://localhost:8000")

socket.addEventListener("open", (event) => {
  socket.send("Hello Server!");
});

setTimeout(()=>{
socket.send("msg")
},12000)

socket.addEventListener("message", (event) =>{
console.log(event.data)
})
socket.addEventListener("error", (event) => {
  socket.send("Error Occurred ", event);
});

