import React from 'react';
import Typography from '@mui/material/Typography';
import { Button } from '@mui/material';
import { HtmlTooltip } from './HtmlTooltip';

  export const AnalogChannelType = () => {
    return (
        <HtmlTooltip
          title={
            <React.Fragment>
              <Typography align="center" style={{color:"#00ff00"}}>Analog Channel number</Typography>
            </React.Fragment>
          }
        >
            <Button style={{textTransform: 'none'}}>Analog Channel</Button>
        </HtmlTooltip>
    );
  };
