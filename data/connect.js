document.addEventListener('DOMContentLoaded', function () {
    const scannerWiFiApi = '/scannerWiFi';
    const statusWiFiApi = '/statusWiFi';
    const restartApi = '/restartESP';
    const localIPApi = '/localIP';
    const disconnectApi = '/disconnectWiFi';

    Vue.component('modal', {
        template: '#modal-template',
        data: function () {
            return {
                statusConnection: null,
                statusWiFi: null,
                getStatusWiFiId: null,
                getStatusId: null,
                getLocalIPId: null,
                wiFiLocalIP: false,
                modalTemplate: null,
                isActive: false,
            };
        },
        methods: {
            check: function () {
                this.statusConnection = "wait, connection";
                document.getElementById("statusСonnection").style.display = "inline-block";
                this.isActive = true;
                setTimeout(() => this.restart(), 7000);
                setTimeout(() => this.getStatusWiFiId = setInterval(() => this.getStatusWiFi(), 2000), 14000);
                setTimeout(() => this.getStatusId = setInterval(() => this.getStatus(), 2000), 15000);
                setTimeout(() => this.stop(), 40000);
            },
            getStatusWiFi: function () {
                fetch(statusWiFiApi)
                    .then(response => response.json())
                    .then(statusWiFi => this.statusWiFi = statusWiFi.statusWiFi);
            },
            restart: function () {
                let restart = new XMLHttpRequest();
                restart.open('GET', restartApi);
                restart.send();
            },
            getStatus: function () {
                if (this.statusWiFi) {
                    clearInterval(this.getStatusWiFiId);
                    clearInterval(this.getStatusId);
                    this.statusConnection = "success";
                    this.isActive = false;
                    this.$parent.statusWiFi = true;
                    this.getLocalIPId = setInterval(() => this.getLocalIP(), 1000)
                }
            },
            stop: function () {
                if (!this.statusWiFi) {
                    clearInterval(this.getStatusWiFiId);
                    clearInterval(this.getStatusId);
                    this.statusConnection = "fail";
                    this.isActive = false;
                }
            },
            getLocalIP: function () {
                fetch(localIPApi)
                    .then(response => response.json())
                    .then(localIP => this.wiFiLocalIP = localIP.wiFiLocalIP);
                if (this.wiFiLocalIP) {
                    clearInterval(this.getLocalIPId);
                    // alert("Local ip: " + this.wiFiLocalIP);
                }
            },
        }
    });

    var appScannerWiFi = new Vue({
        el: '#app-scannerWiFi',
        data: {
            showModal: false,
            networkName: null,
            networks: [],
            idSetInterval: null,
            statusWiFi: false,
        },
        created() {
            this.scannerWiFi();
            this.getStatusWiFi();
            this.idSetInterval = setInterval(this.scannerWiFi, 10000);
        },
        methods: {
            scannerWiFi: function () {
                fetch(scannerWiFiApi)
                    .then(response => response.json())
                    .then(scannerWiFi => this.networks = scannerWiFi.networks)
            },
            getStatusWiFi: function () {
                fetch(statusWiFiApi)
                    .then(response => response.json())
                    .then(statusWiFi => this.statusWiFi = statusWiFi.statusWiFi);
            },
        }
    });

    var appControl = new Vue({
        el: '#app-control',
        data: {
            wiFiLocalIP: '0.0.0.0',

        },
        created() {
            this.getLocalIP();
            setInterval(this.getLocalIP, 10000);
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
