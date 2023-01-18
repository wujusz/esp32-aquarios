import { FC } from 'react';
import { Route, Routes } from 'react-router-dom';
import { useLayoutTitle } from '../../components';
import ChannelListForm from './channels/ChannelList';
import ChannelDetails from './channels/ChannelDetails';
import CustomTheme from '../../CustomTheme';

const Schedule: FC = () => {
  useLayoutTitle("Channel details");

  return (
    <CustomTheme>
      <Routes>
        <Route path="/channel/:id" element={<ChannelDetails />} />
        <Route path="/*" element={<ChannelListForm />} />
      </Routes>
    </CustomTheme>
  );
};

export default Schedule;
