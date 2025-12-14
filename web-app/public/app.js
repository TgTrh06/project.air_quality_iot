const socket = io();

const temp = document.getElementById("temp");
const hum = document.getElementById("hum");
const co2 = document.getElementById("co2");

const ctx = document.getElementById("co2Chart");
const chart = new Chart(ctx, {
    type: "line",
    data: {
        labels: [],
        datasets: [{
            label: "CO₂ ppm",
            data: [],
            borderWidth: 2,
            tension: 0.3
        }]
    },
    options: {
        scales: {
            y: { min: 400, max: 2000 }
        }
    }
});

socket.on("sensor", data => {
    if (!data.co2_ppm) return;

    temp.innerText = data.temperature.toFixed(1);
    hum.innerText = data.humidity.toFixed(1);
    co2.innerText = data.co2_ppm.toFixed(0);

    const t = new Date().toLocaleTimeString();
    chart.data.labels.push(t);
    chart.data.datasets[0].data.push(data.co2_ppm);

    if (chart.data.labels.length > 20) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
    }

    chart.update();
});

function control(on) {
    fetch("/api/control", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            buzzer: on ? 1 : 0,
            led: on ? 1 : 0
        })
    });
}