import { FC } from 'react';
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

  useLayoutTitle("Status");
  return (
    <>
      <div className={classes.flexContainer}>
        {ChannelApi.channels.map((channelId) => {
          return (
            <div className={classes.flexChild}>
              <div>
                <div className={classes.muiListItemGutters}><ChannelStateWebSocketForm channelId={channelId} /> </div>
              </div>
            </div>
          );
        })}
      </div>
    </>
  );
};

export default Status;
