import MenuItem from "@mui/material/MenuItem";
import Select from "@mui/material/Select";
import { BlockFormControlLabel } from '../../../../../components';
import { ChannelState } from '../../../redux/types/channel';
import { AnalogChannelType } from "../../tooltips/AnalogChannelType";

export function selectAnalogChannel(data: ChannelState,
  handleChannelStateValueChange: (name: keyof ChannelState) => (event: any) => void) {
  return (<BlockFormControlLabel
    control={<Select
      style={{ marginLeft: 10, height: 30 }}
      value={data.analogChannel}
      onChange={handleChannelStateValueChange('analogChannel')}
             >
      <MenuItem value={0}>0</MenuItem>
      <MenuItem value={1}>1</MenuItem>
      <MenuItem value={2}>2</MenuItem>
      <MenuItem value={3}>3</MenuItem>
      <MenuItem value={4}>4</MenuItem>
      <MenuItem value={5}>5</MenuItem>
      <MenuItem value={6}>6</MenuItem>
      <MenuItem value={7}>7</MenuItem>
      <MenuItem value={8}>8</MenuItem>
      <MenuItem value={9}>9</MenuItem>
      <MenuItem value={10}>10</MenuItem>
      <MenuItem value={11}>11</MenuItem>
      <MenuItem value={12}>12</MenuItem>
      <MenuItem value={13}>13</MenuItem>
      <MenuItem value={14}>14</MenuItem>
      <MenuItem value={15}>15</MenuItem>
             </Select>}
    label={(<AnalogChannelType />)}
          />);
}
