import { FC, useEffect, useState } from 'react';
import { Theme } from '@mui/material';
import { makeStyles, createStyles } from "@mui/styles";
import { useLayoutTitle } from '../../components';
import * as ChannelApi from './api/channelApi';
import ChannelStateWebSocketForm from './channels/ws/ChannelStateWebSocketForm';

const useStyles = makeStyles((theme: Theme) => createStyles({
  flexContainer: {
    display: "flex",
    flexWrap: "wrap",
  },

  flexChild: {
    flex: "1",
    margin: theme.spacing(1),
  },

  muiListItemGutters: {
    width: "100%",
    margin: theme.spacing(1),
  },
}));

const Status: FC = () => {
  const classes = useStyles();
  const [channels, setChannels] = useState<string[]>([]);

  useLayoutTitle("Status");

  useEffect(() => {
    ChannelApi.fetchChannelListApi()
      .then((resp) => setChannels(resp.data))
      .catch((e) => console.error(e));
  }, []);

  return (
    <>
      <div className={classes.flexContainer}>
        {channels.map((channelId) => (
          <div className={classes.flexChild} key={channelId}>
            <div className={classes.muiListItemGutters}>
              <ChannelStateWebSocketForm channelId={channelId} />
            </div>
          </div>
        ))}
      </div>
    </>
  );
};

export default Status;
