const socket = io();

const temp = document.getElementById("temp");
const hum = document.getElementById("hum");
const co2 = document.getElementById("co2");

Chart.defaults.color = '#94a3b8';
Chart.defaults.borderColor = 'rgba(255, 255, 255, 0.1)';

const ctx = document.getElementById("co2Chart").getContext('2d');


const chart = new Chart(ctx, {
    type: "line",
    data: {
        labels: [],
        datasets: [{
            label: "CO₂ ppm",
            data: [],
            borderWidth: 3,
            borderColor: '#4ade80', // Green
            backgroundColor: 'rgba(74, 222, 128, 0.1)', // Solid low opacity
            fill: true,
            tension: 0.4,
            pointBackgroundColor: '#4ade80',
            pointBorderColor: '#fff',
            pointRadius: 4,
            pointHoverRadius: 6
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                labels: {
                    font: {
                        family: "'Outfit', sans-serif",
                        size: 14
                    }
                }
            },
            tooltip: {
                backgroundColor: 'rgba(15, 23, 42, 0.9)',
                titleColor: '#f8fafc',
                bodyColor: '#cbd5e1',
                padding: 10,
                cornerRadius: 8,
                displayColors: false
            }
        },
        scales: {
            x: {
                grid: {
                    display: false
                },
                ticks: {
                    maxTicksLimit: 8
                }
            },
            y: {
                min: 400,
                max: 2000,
                grid: {
                    color: 'rgba(255, 255, 255, 0.05)'
                },
                border: {
                    display: false
                }
            }
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

    // Sync button state from server if available
    if (data.buzzer_on !== undefined) {
        updateBuzzerUI(data.buzzer_on);
    }
});

function updateBuzzerUI(on) {
    const btnAlert = document.getElementById('btn-alert');
    const btnNormal = document.getElementById('btn-normal');

    if (on) {
        btnAlert.classList.add('active');
        btnNormal.classList.remove('active');
    } else {
        btnAlert.classList.remove('active');
        btnNormal.classList.add('active');
    }
}

function control(on) {
    // Optimistic UI update
    updateBuzzerUI(on);

    // Send command
    fetch("/api/control", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            buzzer: on ? 1 : 0,
            led: on ? 1 : 0
        })
    });
}