var activeCount = 5;

$('#addPot').on('click', () => {
  $.ajax({ 
    type: 'POST',
    url: '/pots/add',
    data: {
      potName: document.getElementById('addPot-name').value,
      potId: document.getElementById('addPot-Id').value 
    },
    success: function (data) { 
      console.log(data);
      location.reload(); 
    }
  });
});

function initChart() {
  var $potChart = $('.pot .lineChart');
  if ($potChart.length) {
    var ctxL = $potChart[0].getContext('2d');
  
    GPot.Chart = new Chart(ctxL, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
            label: "Temperature (°C)",
            typex: "deg",
            data: [],
            yAxisID: 'y-axis-1',
            borderColor: [
              'rgba(200, 99, 132, .7)',
            ],
            borderWidth: 2
          },
          {
            label: "Humidity (%)",
            typex: "per",
            data: [],
            yAxisID: 'y-axis-2',
            borderColor: [
              'rgba(0, 10, 130, .7)',
            ],
            borderWidth: 2
          }
        ]
      },
      options: {
        responsive: true,
        hoverMode: 'index',
        stacked: false,
        scales: {
          yAxes: [{
            scaleLabel: {
              display: true,
              labelString: 'Temperature',
            },
            type: 'linear', // only linear but allow scale type registration. This allows extensions to exist solely for log scale for instance
            display: true,
            position: 'left',
            id: 'y-axis-1',
            ticks: {
              max: 50,
              min: 0,
              stepSize: 5
            }
          }, {
            scaleLabel: {
              display: true,
              labelString: 'Humidity',
            },
            type: 'linear', // only linear but allow scale type registration. This allows extensions to exist solely for log scale for instance
            display: true,
            position: 'right',
            id: 'y-axis-2',
            ticks: {
              max: 100,
              min: 0,
              stepSize: 10
            },
            // grid line settings
            gridLines: {
              drawOnChartArea: false, // only want the grid lines for one axis to show up
            },
          }],
        }
      }
    });
  }
}

function updateChart() {
  GPot.Chart.data.labels.push(moment(GPotLog.TimeStamp*1000).format('MM:ss'));
  if (GPot.Chart.data.labels.length > 20) {
    GPot.Chart.data.labels.shift();
  }
  GPot.Chart.data.datasets.forEach((dataset) => {
    if (dataset.data.length > 20) {
      dataset.data.shift();
    }
    if (dataset.typex == "per") {
      dataset.data.push(GPotLog.CurSoil);
    } else {
      dataset.data.push(GPotLog.CurTemp);
    }
  });
  GPot.Chart.update();
}

function updatePot() {
  var pot = GPot.Pot;
  if (pot) {
    $('.potList').removeClass('e-hide');
    document.getElementById('autoPumpSwitch').checked = pot.IsAutoPump;
    document.getElementById('activePump').checked = pot.IsPump;
    if (pot.IsPump) {
      $('#manualPump').removeClass('btn-light');
      $('#manualPump').addClass('btn-primary');
    } else {
      $('#manualPump').removeClass('btn-primary');
      $('#manualPump').addClass('btn-light');
    }
  }
  if (GPotLog) {
    document.getElementById('UpdateAt').textContent = moment(GPotLog.TimeStamp*1000).format('DD, MMM, YYYY hh:mm:ss');
    document.getElementById('curSoil').textContent = GPotLog.CurSoil;
  }
}

function updateInfo() {
  if (GPot.Pot.PotId) {
    $.ajax({ 
      type: 'GET',
      url: '/pots/info?web=true&id=' + GPot.Pot.PotId,
      data: { },
      success: function (data) { 
        var pot = data.Pot;
        var potLog = data.PotLog;
        if (pot) {
          GPot.Pot = pot;
          updatePot();
        }
        if (potLog) {
          if (potLog.TimeStamp != GPotLog.TimeStamp) {
            GPotLog = potLog;
            document.getElementById('activePot').checked = true;
            updateChart();
          } else {
            if (activeCount >= 5) {
              document.getElementById('activePot').checked = false;
              activeCount = 0;
            }
            activeCount++;
          }
        }
        console.log(GPotLog);
      }
    });
  }
}

function sendInfo(data, url=null) {
  $.ajax({ 
    type: 'POST',
    url: '/pots/wupdate',
    data: data,
    success: function (data) { 
      if (url) {
        // window.location.href = url;
        location.reload(); 
      }
      console.log(data);
    }
  });
};

$('#updatePumpLevel').on('click', () => {
  sendInfo(
    { id: GPot.Pot._id, 
      'HighPumpLevel': document.getElementById('highPumpLevel').value, 
      'LowPumpLevel': document.getElementById('lowPumpLevel').value
    }
  );
});

$('#autoPumpSwitch').on('click', () => {
  sendInfo(
    { id: GPot.Pot._id, 
      'IsAutoPump': document.getElementById('autoPumpSwitch').checked
    }
  );
});

$('#resetSetting').on('click', () => {
  sendInfo(
    { id: GPot.Pot._id, 
      'ResetWifi': GPotLog.TimeStamp
    }
  );
});

$('#removePot').on('click', () => {
  sendInfo(
    { id: GPot.Pot._id, 
      'Remove': true
    },
    '/pots'
  );
});

$('#manualPump').on('click', () => {
  sendInfo(
    { id: GPot.Pot._id, 
      'IsPump': !GPot.Pot.IsPump,
      'IsAutoPump': false
    }
  );
});

$('#potName').keypress(function(event){
  var keycode = (event.keyCode ? event.keyCode : event.which);
  if(keycode == '13'){
    sendInfo(
      { id: GPot.Pot._id, 
        'Name': document.getElementById('potName').value
      }
    );
  }
});