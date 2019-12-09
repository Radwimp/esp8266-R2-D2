document.addEventListener('DOMContentLoaded', function () {
    const scannerWiFiApi = '/scannerWiFi';
    const statusWiFiApi = '/statusWiFi';
    const restartApi = '/restartESP';
    const localIPApi = '/localIP';
    const disconnectApi = '/disconnectWiFi';

    var appControl = new Vue({
        el: '#app-control',
        data: {
            wiFiLocalIP: '0.0.0.0',
            localIpId: null,

        },
        created() {
            this.getLocalIP();
            this.localIpId = setInterval(() => this.getLocalIP, 20000);
        },
        methods: {
            getLocalIP: function () {
                fetch(localIPApi)
                    .then(response => response.json())
                    .then(localIP => this.wiFiLocalIP = localIP.wiFiLocalIP);
            },
            disconnect: function () {
                let disconnect = new XMLHttpRequest();
                disconnect.open('GET', disconnectApi);
                disconnect.send();
                let restart = new XMLHttpRequest();
                restart.open('GET', restartApi);
                restart.send();
                document.location.reload(true);
            }
        }
    });

});
