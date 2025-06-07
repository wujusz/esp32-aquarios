import { FC, useEffect, useState } from 'react';
import { IconButton, Table, TableBody, TableCell, TableHead, TableRow } from '@mui/material';
import EditIcon from '@mui/icons-material/Edit';
import AddModuleForm from '../channels/AddModuleForm';
import * as ModuleApi from '../api/moduleApi';
import { useNavigate } from 'react-router-dom';
import { SectionContent, useLayoutTitle } from '../../../components';
import { RemoteUtils } from '../utils/remoteUtils';

const ModuleList: FC = () => {
  const [modules, setModules] = useState<any[]>([]);
  const navigate = useNavigate();

  useEffect(() => {
    ModuleApi.fetchModuleListApi()
      .then((resp) => setModules(resp.data))
      .catch((e) => console.error(e));
  }, []);

  useLayoutTitle('Modules');

  const showModule = (id: string) => {
    navigate(RemoteUtils.getNavigationLink('auto', `module/${id}`));
  };

  return (
    <SectionContent title='Modules' titleGutter>
      <Table size='small'>
        <TableHead>
          <TableRow>
            <TableCell>Name</TableCell>
            <TableCell>Type</TableCell>
            <TableCell align='center'>Channels</TableCell>
            <TableCell></TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {modules.map((m) => (
            <TableRow key={m.id}>
              <TableCell>{m.name}</TableCell>
              <TableCell>{m.type}</TableCell>
              <TableCell align='center'>{m.channels}</TableCell>
              <TableCell align='center'>
                <IconButton size='small' onClick={() => showModule(m.id)}>
                  <EditIcon />
                </IconButton>
              </TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
      <AddModuleForm />
    </SectionContent>
  );
};

export default ModuleList;
