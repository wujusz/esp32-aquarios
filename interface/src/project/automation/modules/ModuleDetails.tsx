import { FC, useEffect, useState } from 'react';
import { useParams } from 'react-router-dom';
import { Table, TableBody, TableCell, TableHead, TableRow, TextField, Button, Checkbox, FormControlLabel } from '@mui/material';
import * as ModuleApi from '../api/moduleApi';
import { useLayoutTitle, SectionContent } from '../../../components';
import { ModuleScheduleForm, defaultScheduleState, ModuleScheduleState } from './ModuleScheduleForm';
import SensorChart from '../sensors/SensorChart';

interface ModuleData {
  id: string;
  name: string;
  type: string;
  useSun?: boolean;
  targetTemp?: number;
  schedules: ModuleScheduleState[];
  channels: { id: string; pin: number; brightness?: number }[];
}

const ModuleDetails: FC = () => {
  const { id } = useParams();
  const [module, setModule] = useState<ModuleData | null>(null);

  useLayoutTitle('Module details');

  useEffect(() => {
    if (id) {
      ModuleApi.fetchModuleApi(id)
        .then((resp) => {
          const data = resp.data;
          if (!data.schedules && data.schedule) {
            data.schedules = [ { ...defaultScheduleState(), schedule: data.schedule } ];
          }
          if (!data.schedules) data.schedules = [defaultScheduleState()];
          setModule(data);
        })
        .catch((e) => console.error(e));
    }
  }, [id]);

  const save = async () => {
    if (module) {
      try {
        const payload = { ...module, schedules: module.schedules.map((s) => s.schedule) };
        await ModuleApi.updateModuleApi(payload);
        window.location.reload();
      } catch (e) {
        console.error(e);
      }
    }
  };

  if (!module) return null;

  const addChannel = () => {
    setModule({ ...module, channels: [...module.channels, { id: '', pin: 0, brightness: 0 }] });
  };

  const addSchedule = () => {
    setModule({ ...module, schedules: [...module.schedules, defaultScheduleState()] });
  };

  const updateSchedule = (idx: number, value: ModuleScheduleState) => {
    const sc = [...module.schedules];
    sc[idx] = value;
    setModule({ ...module, schedules: sc });
  };

  const removeSchedule = (idx: number) => {
    const sc = module.schedules.filter((_s, i) => i !== idx);
    setModule({ ...module, schedules: sc });
  };

  const updateChannel = (idx: number, field: 'id' | 'pin' | 'brightness', value: string) => {
    const ch = [...module.channels];
    if (field === 'id') ch[idx].id = value;
    else if (field === 'pin') ch[idx].pin = parseInt(value, 10);
    else ch[idx].brightness = parseInt(value, 10);
    setModule({ ...module, channels: ch });
  };

  const removeChannel = (idx: number) => {
    const ch = module.channels.filter((_c, i) => i !== idx);
    setModule({ ...module, channels: ch });
  };

  return (
    <SectionContent title={`Module ${module.id}`} titleGutter>
      <TextField label='Name' size='small' value={module.name} onChange={(e) => setModule({ ...module, name: e.target.value })} style={{ marginRight: 8 }} />
      <FormControlLabel control={<Checkbox checked={module.useSun ?? false} onChange={(e) => setModule({ ...module, useSun: e.target.checked })} />} label='Use Sunrise/Sunset' />
      {module.type === 'thermometer' && (
        <TextField label='Target °C' size='small' type='number' value={module.targetTemp ?? ''} onChange={(e) => setModule({ ...module, targetTemp: parseFloat(e.target.value) })} style={{ marginLeft: 8 }} />
      )}
      {module.schedules.map((s, idx) => (
        <div key={idx} style={{ marginTop: 8 }}>
          <ModuleScheduleForm value={s} onChange={(v) => updateSchedule(idx, v)} />
          <Button variant='outlined' color='error' size='small' onClick={() => removeSchedule(idx)} style={{marginTop: 4}}>Delete</Button>
        </div>
      ))}
      <Button variant='outlined' size='small' onClick={addSchedule} style={{ marginTop: 8 }}>Add Schedule</Button>
      <Table size='small' style={{ marginTop: 8 }}>
        <TableHead>
          <TableRow>
            <TableCell>Channel Id</TableCell>
            <TableCell>GPIO Pin</TableCell>
            {module.type === 'lighting' && <TableCell>Brightness</TableCell>}
            <TableCell></TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {module.channels.map((c, idx) => (
            <TableRow key={idx}>
              <TableCell>
                <TextField size='small' value={c.id} onChange={(e) => updateChannel(idx, 'id', e.target.value)} />
              </TableCell>
              <TableCell>
                <TextField size='small' type='number' value={c.pin} onChange={(e) => updateChannel(idx, 'pin', e.target.value)} />
              </TableCell>
              {module.type === 'lighting' && (
                <TableCell>
                  <TextField size='small' type='number' value={c.brightness ?? 0} onChange={(e) => updateChannel(idx, 'brightness', e.target.value)} style={{ width: 80 }} />
                </TableCell>
              )}
              <TableCell>
                <Button variant='outlined' color='error' onClick={() => removeChannel(idx)}>Delete</Button>
              </TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
      <Button variant='contained' onClick={addChannel} style={{ marginTop: 8 }}>Add Channel</Button>
      <div style={{ marginTop: 12 }}>
        <Button variant='contained' color='primary' onClick={save}>Save</Button>
      </div>
      {module.type === 'thermometer' && (
        <div style={{ marginTop: 20 }}>
          <SensorChart id={`temp_${module.id}`} />
        </div>
      )}
    </SectionContent>
  );
};

export default ModuleDetails;
