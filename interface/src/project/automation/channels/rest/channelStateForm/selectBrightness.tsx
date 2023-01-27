import { Typography } from '@mui/material';
import Slider from '@mui/material/Slider';
import { ChannelState } from '../../../redux/types/channel';
import { HotTime } from '../../tooltips/HotTime';

function selectBrightnessMarks() {
  let markList = [];
  let numbers = [0,10,20,30,40,50,60,70,80,90,100];
  for(let number of numbers)
  {
    markList.push({
      value: number,
      label: `${number}%`
    });
  }

  return markList;
}



export function selectBrightness(data: ChannelState, 
handleBrightnessChange: (event: any, newValue: number | number[]) => void) {
  let marks = selectBrightnessMarks();
  return (<div>
    <Typography id="discrete-slider" gutterBottom>
      {data.brightness}%
    </Typography>
    <Slider
      disabled={!data.enabled}
      defaultValue={data.brightness}
      aria-labelledby="discrete-slider-custom"
      step={10}
      valueLabelDisplay="off"
      marks={marks}
      min={0}
      max={100}
      onChange={handleBrightnessChange}
    />
          </div>);
}
