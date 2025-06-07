import { FC, useEffect, useState } from 'react';
import { Line } from 'react-chartjs-2';
import { Chart, LineElement, LinearScale, PointElement, CategoryScale } from 'chart.js';

Chart.register(LineElement, LinearScale, PointElement, CategoryScale);

interface Props {
  id: string;
}

const SensorChart: FC<Props> = ({ id }) => {
  const [data, setData] = useState<{ t: number; v: number }[]>([]);
  useEffect(() => {
    fetch(`/rest/sensorLog?id=${id}`)
      .then(r => r.json())
      .then(setData)
      .catch(() => {});
  }, [id]);

  const chartData = {
    labels: data.map(d => new Date(d.t * 1000).toLocaleTimeString()),
    datasets: [{
      label: id,
      data: data.map(d => d.v),
      borderColor: 'blue',
      fill: false,
    }],
  };

  return <Line data={chartData} />;
};

export default SensorChart;
