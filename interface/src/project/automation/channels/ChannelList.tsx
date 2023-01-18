
import React, { FC, useEffect, useState } from 'react';
import Box from '@mui/material/Box';
import Tab from '@mui/material/Tab';
import TabContext from '@mui/lab/TabContext';
import TabList from '@mui/lab/TabList';
import TabPanel from '@mui/lab/TabPanel';
import { ThemeProvider } from '@mui/material/styles';
import type { } from '@mui/lab/themeAugmentation';
import { MuiThemeOverride } from './themeOverrides';
import { useNavigate } from 'react-router-dom';
import { useLayoutTitle, ButtonRow, FormLoader, MessageBox, SectionContent } from '../../../components';

import {
  Button, IconButton, Table, TableBody, TableCell, TableFooter, TableHead, TableRow
} from '@mui/material';
import EditIcon from '@mui/icons-material/Edit';
import CheckIcon from '@mui/icons-material/Check';
import CloseIcon from '@mui/icons-material/Close';
import { Link } from 'react-router-dom';

import * as ChannelApi from '../api/channelApi';
import { RemoteUtils } from '../utils/remoteUtils';

const ChannelListForm: FC = () => {
  const [ list, setList ] = useState<any[]>([]);
  const navigate = useNavigate();  
  const channelUrl = (path: string) => navigate(RemoteUtils.getNavigationLink('auto', path));
  const getChannelData = async (channelId: string) => {
    try {
      const response = await ChannelApi.createReadChannelApi(channelId);
      response.data.channelId = channelId;
        return response.data;
      } catch (error) {
        console.error(error);
      }
  };

  useEffect(() => {
    Promise.all(ChannelApi.channels.map((channel) => getChannelData(channel))).then((resolvedChannels) => {
      setList(resolvedChannels)
    })
  }, [])

  useLayoutTitle("Automation");

  const content = () => {
    if (!list) {
      return;
    }

    const showChannel = (channelUrl: string) => {
      console.log('showChannel', channelUrl);
      return navigate(RemoteUtils.getNavigationLink('auto', channelUrl));
    }

    return (
      <>
          <Table size="small">
            <TableHead>
            <TableRow>
              <TableCell>Channel name</TableCell>
              <TableCell align="center">Status</TableCell>
              <TableCell align="center">Enabled</TableCell>
              <TableCell align="center">PIN</TableCell>
              <TableCell align="center">Type</TableCell>
              <TableCell></TableCell>
            </TableRow>
            </TableHead>
            <TableBody>
              {list.map((channel) => (
                <TableRow key={channel.uniqueId}>
                  <TableCell component="th" scope="row">
                    <a onClick={() => channelUrl(channel.restChannelEndPoint)}>{channel.name}</a>
                  </TableCell>
                  <TableCell align="center">
                    {
                      channel.controlOn ? <CheckIcon /> : <CloseIcon />
                    }
                  </TableCell>
                  <TableCell align="center">
                    {
                      channel.enabled ? <CheckIcon /> : <CloseIcon />
                    }
                  </TableCell>
                  <TableCell align='center'>{channel.controlPin}</TableCell>
                  <TableCell align='center'>{channel.homeAssistantTopicType}</TableCell>
                  <TableCell align="center">
                    <IconButton size="small" aria-label="Show" onClick={() => showChannel(channel)}>
                      <EditIcon />
                    </IconButton>
                  </TableCell>
                </TableRow>
              ))}  
            </TableBody>
            <TableFooter >
              <TableRow>

              </TableRow>
            </TableFooter>
          </Table>
      </>
    );
  }

  return (
    <SectionContent title='Manage Channels' titleGutter>
      {content()}
    </SectionContent>
  );


};

export default ChannelListForm;
