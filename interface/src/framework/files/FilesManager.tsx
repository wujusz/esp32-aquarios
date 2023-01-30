import React, { FC, useContext } from 'react';
import { Navigate, Route, Routes } from 'react-router-dom';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from '../../components';
import { AuthenticatedContext } from '../../contexts/authentication';


const FilesManager: FC = () => {
  useLayoutTitle("File manager");

  console.log('fileManager loaded');

  return (
    <>
      
    </>
  );

};

export default FilesManager;
