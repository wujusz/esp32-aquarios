import { FC, useState } from 'react';
import { TextField, Button } from '@mui/material';
import * as ChannelApi from '../api/channelApi';

const AddChannelForm: FC = () => {
  const [channelId, setChannelId] = useState('');
  const [pin, setPin] = useState('');

  const addChannel = async () => {
    if (channelId && pin) {
      try {
        await ChannelApi.createAddChannelApi(channelId, parseInt(pin, 10));
        window.location.reload();
      } catch (e) {
        console.error(e);
      }
    }
  };

  return (
    <div style={{ marginTop: 20 }}>
      <TextField label='Channel Id' size='small' value={channelId} onChange={(e) => setChannelId(e.target.value)} style={{marginRight: 8}} />
      <TextField label='GPIO Pin' size='small' type='number' value={pin} onChange={(e) => setPin(e.target.value)} style={{marginRight: 8}} />
      <Button variant='contained' onClick={addChannel}>Add Channel</Button>
    </div>
  );
};

export default AddChannelForm;
