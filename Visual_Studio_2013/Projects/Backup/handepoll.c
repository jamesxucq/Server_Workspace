/*
            if (!evd->loop_continue) session->subentry = GetSubentry(psbuffer->head_buffer, psbuffer->hlen);
            switch (session->subentry) {
                case TYPE_ANCHOR:
                    if (!evd->loop_continue) {
                        if (ParseGetAnchorRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParseGetAnchorRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        GetAnchorRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenGetAnchorSe(psctrl, session->list_inform, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                    }
                    break;
                case TYPE_FILE:
                    if (!evd->loop_continue) {
                        if (ParseGetFileRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParseGetFileRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        GetFileRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenGetFileSe(psctrl, session->list_inform, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                    }
                    break;
                case TYPE_LIST:
                    if (!evd->loop_continue) {
                        if (ParseGetListRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParseGetListRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        GetListRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenGetListSe(psctrl, session, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                    }
                    break;
                case TYPE_SUMS:
                    if (!evd->loop_continue) {
                        if (ParseGetSumsRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParseGetSumsRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        GetSumsRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenGetSumsSe(psctrl, session->list_inform, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                    }
                    break;
            }
            break;
 */

/*
            if (!evd->loop_continue) session->subentry = PutSubentry(psbuffer->head_buffer, psbuffer->hlen);
            switch (session->subentry) {
                case TYPE_FILE:
                    if (!evd->loop_continue) {
                        if (ParsePutFileRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParsePutFileRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        PutFileRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenPutFileSe(psctrl, session->list_inform, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        if (STATUS_OK != psctrl->sdtp_status) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandlePutFileRecv(psvalue, psctrl, psbuffer)) {
                        ClosePutFileSe(psctrl);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
                case TYPE_SUMS:
                    if (!evd->loop_continue) {
                        if (ParsePutSumsRecv(&RequestValue, psbuffer->head_buffer)) {
                            LOG_INFO("-------- ParsePutSumsRecv false --------");
                            psctrl->sdtp_status = STATUS_BAD_REQUEST;
                            break;
                        }
                        PutSumsRequest(psvalue, &RequestValue);
                        if (strcmp(psctrl->session_id, psvalue->session_id)) {
                            psctrl->sdtp_status = STATUS_UNAUTHORIZED;
                            break;
                        } else psctrl->sdtp_status = STATUS_OK;
                        if (OpenPutSumsSe(psctrl, psvalue))
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        if (STATUS_OK != psctrl->sdtp_status) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandlePutSumsRecv(psvalue, psctrl, psbuffer)) {
                        MATCH_FILE_CHUNK_SUMS(psctrl, session->list_inform, psvalue)
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
            }
            break;
 */



/*
            switch (session->subentry) {
                case TYPE_ANCHOR:
                    if (!evd->loop_continue) {
                        GetAnchorResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                        if (BuildGetAnchorSend(psbuffer, &ResponseValue)) {
                            LOG_INFO("-------- BuildGetAnchorSend false --------");
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        }
                        if (STATUS_OK != psctrl->sdtp_status || 0 == psvalue->content_length) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandleGetAnchorSend(psbuffer, psvalue, psctrl)) {
                        CloseGetAnchorSe(psctrl, psvalue);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
                case TYPE_FILE:
                    if (!evd->loop_continue) {
                        GetFileResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                        if (BuildGetFileSend(psbuffer, &ResponseValue)) {
                            LOG_INFO("-------- BuildGetFileSend false --------");
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        }
                        if (STATUS_OK != psctrl->sdtp_status || 0 == psvalue->content_length) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandleGetFileSend(psbuffer, psvalue, psctrl)) {
                        LOG_INFO("get file loop stop. epoll [fd:%d]", evd->sockfd);
                        CloseGetFileSe(psctrl);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
                case TYPE_LIST:
                    if (!evd->loop_continue) {
                        GetListResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                        if (BuildGetListSend(psbuffer, &ResponseValue)) {
                            LOG_INFO("-------- BuildGetListSend false --------");
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        }
                        if (STATUS_OK != psctrl->sdtp_status || 0 == psvalue->content_length) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandleGetListSend(psbuffer, psvalue, psctrl)) {
                        CloseGetListSe(psctrl);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
                case TYPE_SUMS:
                    if (!evd->loop_continue) {
                        GetSumsResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                        if (BuildGetSumsSend(psbuffer, &ResponseValue)) {
                            LOG_INFO("-------- BuildGetFileSend false --------");
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        }
                        if (STATUS_OK != psctrl->sdtp_status || 0 == psvalue->content_length) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandleGetSumsSend(psbuffer, psvalue, psctrl)) {
                        CloseGetSumsSe(psctrl, psvalue);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
            }
            break;
 */

/*
            switch (session->subentry) {
                case TYPE_FILE:
                    PutFileResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                    if (BuildPutFileSend(psbuffer, &ResponseValue)) {
                        LOG_INFO("-------- BuildPutFileSend false --------");
                        psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                    }
                    set_file_modtime_ext(session->list_inform->location, psvalue->file_name, psvalue->last_write);
                    AddNewChangeBzl(session->anch_opt->anch_datfp, psvalue->file_name, ADD);
                    //evd->loop_continue = LOOP_STOP;                    
                    break;
                case TYPE_SUMS:
                    if (!evd->loop_continue) {
                        PutSumsResponse(&ResponseValue, psctrl->sdtp_status, psvalue);
                        if (BuildPutSumsSend(psbuffer, &ResponseValue)) {
                            LOG_INFO("-------- BuildPutFileSend false --------");
                            psctrl->sdtp_status = STATUS_INTERNAL_SERVER_ERROR;
                        }
                        if (STATUS_OK != psctrl->sdtp_status || 0 == psvalue->content_length) break;
                        evd->loop_continue |= LOOP_CONTINUE;
                    }
                    if (!HandlePutSumsSend(psbuffer, psvalue, psctrl)) {
                        ClosePutSumsSe(psctrl);
                        evd->loop_continue &= LOOP_STOP;
                    }
                    break;
            }
            break;
 */