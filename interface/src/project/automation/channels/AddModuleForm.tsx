import { FC, useState } from 'react';
import { TextField, Button, MenuItem, Select, Checkbox, FormControlLabel } from '@mui/material';
import * as ModuleApi from '../api/moduleApi';
import { ModuleScheduleForm, defaultScheduleState, ModuleScheduleState } from '../modules/ModuleScheduleForm';

const AddModuleForm: FC = () => {
  const [moduleId, setModuleId] = useState('');
  const [moduleType, setModuleType] = useState('lighting');
  const [moduleName, setModuleName] = useState('');
  const [channels, setChannels] = useState('');
  const [useSun, setUseSun] = useState(false);
  const [schedules, setSchedules] = useState<ModuleScheduleState[]>([defaultScheduleState()]);

  const addModule = async () => {
    if (moduleId && channels) {
      const chList = channels.split(',').map((pair) => {
        const [id, pin] = pair.split(':');
        return { id: id.trim(), pin: parseInt(pin, 10) };
      }).filter((c) => c.id && !isNaN(c.pin));
      try {
        await ModuleApi.createAddModuleApi(
          moduleId,
          moduleType,
          moduleName,
          chList,
          schedules.map((s) => s.schedule),
          useSun
        );
        window.location.reload();
      } catch (e) {
        console.error(e);
      }
    }
  };

  const updateSchedule = (idx: number, value: ModuleScheduleState) => {
    const arr = [...schedules];
    arr[idx] = value;
    setSchedules(arr);
  };

  const addSchedule = () => {
    setSchedules([...schedules, defaultScheduleState()]);
  };

  const removeSchedule = (idx: number) => {
    setSchedules(schedules.filter((_s, i) => i !== idx));
  };

  return (
    <div style={{ marginTop: 20 }}>
      <TextField label='Module Id' size='small' value={moduleId} onChange={(e) => setModuleId(e.target.value)} style={{ marginRight: 8 }} />
      <TextField label='Name' size='small' value={moduleName} onChange={(e) => setModuleName(e.target.value)} style={{ marginRight: 8 }} />
      <Select size='small' value={moduleType} onChange={(e) => setModuleType(e.target.value as string)} style={{ marginRight: 8 }}>
        <MenuItem value='lighting'>Lighting</MenuItem>
        <MenuItem value='switch'>Switch</MenuItem>
        <MenuItem value='sensor'>Sensor</MenuItem>
      </Select>
      <FormControlLabel control={<Checkbox checked={useSun} onChange={(e) => setUseSun(e.target.checked)} />} label='Sunrise/Sunset' style={{marginRight: 8}} />
      <TextField label='Channels (id:pin,...)' size='small' value={channels} onChange={(e) => setChannels(e.target.value)} style={{ marginRight: 8 }} />
      {schedules.map((s, idx) => (
        <div key={idx} style={{ marginTop: 10 }}>
          <ModuleScheduleForm value={s} onChange={(v) => updateSchedule(idx, v)} />
          <Button variant='outlined' color='error' size='small' onClick={() => removeSchedule(idx)} style={{marginTop: 4}}>Delete</Button>
        </div>
      ))}
      <Button variant='outlined' size='small' onClick={addSchedule} style={{ marginTop: 8, marginRight: 8 }}>Add Schedule</Button>
      <Button variant='contained' onClick={addModule} style={{ marginTop: 8 }}>Add Module</Button>
    </div>
  );
};

export default AddModuleForm;
