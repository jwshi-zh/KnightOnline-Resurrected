-- 2002-08-14 오후 12:06에서 생성된 스크립트
-- 계정: sa
-- 서버: 218.50.5.41

BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Tuesday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Tuesday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Tuesday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Tuesday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_TUE', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'Backup Tuesday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 4, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Monday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Monday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Monday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Monday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_MON', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'backup_schedule', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 2, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Thusday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Thusday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Thusday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Thusday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_THU', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'backup thusday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 16, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Sunday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Sunday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Sunday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Sunday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_SUN', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'backup sunday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 1, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Wednesday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Wednesday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Wednesday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Wednesday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_WED', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'Backup Wednesday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 8, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Friday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Friday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Friday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Friday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_FRI', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'backup friday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 32, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
BEGIN TRANSACTION            
  DECLARE @JobID BINARY(16)  
  DECLARE @ReturnCode INT    
  SELECT @ReturnCode = 0     
IF (SELECT COUNT(*) FROM msdb.dbo.syscategories WHERE name = N'[Uncategorized (Local)]') < 1 
  EXECUTE msdb.dbo.sp_add_category @name = N'[Uncategorized (Local)]'

 -- 동일한 이름을 가진 작업이 있으면 삭제
  SELECT @JobID = job_id     
  FROM   msdb.dbo.sysjobs    
  WHERE (name = N'Backup_Saturday')       
  IF (@JobID IS NOT NULL)    
  BEGIN  
  -- 작업이 다중 서버 작업인지 점검  
  IF (EXISTS (SELECT  * 
              FROM    msdb.dbo.sysjobservers 
              WHERE   (job_id = @JobID) AND (server_id <> 0))) 
  BEGIN 
    -- 스크립트를 중단하십시오. 
    RAISERROR (N'이 이름으로 된 다중 서버 작업이 이미 존재하기 때문에 Backup_Saturday 작업을 가져올 수 없습니다.', 16, 1) 
    GOTO QuitWithRollback  
  END 
  ELSE 
    -- [로컬] 작업 삭제 
    EXECUTE msdb.dbo.sp_delete_job @job_name = N'Backup_Saturday' 
    SELECT @JobID = NULL
  END 

BEGIN 

 -- 작업 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_job @job_id = @JobID OUTPUT , @job_name = N'Backup_Saturday', @owner_login_name = N'sa', @description = N'설명이 없습니다.', @category_name = N'[Uncategorized (Local)]', @enabled = 1, @notify_level_email = 0, @notify_level_page = 0, @notify_level_netsend = 0, @notify_level_eventlog = 2, @delete_level= 0
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 단계 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobstep @job_id = @JobID, @step_id = 1, @step_name = N'procedure call', @command = N'exec BACKUP_SAT', @database_name = N'KN_Online', @server = N'', @database_user_name = N'', @subsystem = N'TSQL', @cmdexec_success_code = 0, @flags = 0, @retry_attempts = 0, @retry_interval = 1, @output_file_name = N'', @on_success_step_id = 0, @on_success_action = 1, @on_fail_step_id = 0, @on_fail_action = 2
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 
  EXECUTE @ReturnCode = msdb.dbo.sp_update_job @job_id = @JobID, @start_step_id = 1 

  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 작업 일정 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobschedule @job_id = @JobID, @name = N'backup saturday', @enabled = 1, @freq_type = 8, @active_start_date = 20020726, @active_start_time = 60000, @freq_interval = 64, @freq_subday_type = 1, @freq_subday_interval = 0, @freq_relative_interval = 0, @freq_recurrence_factor = 1, @active_end_date = 99991231, @active_end_time = 235959
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

 -- 대상 서버 추가
  EXECUTE @ReturnCode = msdb.dbo.sp_add_jobserver @job_id = @JobID, @server_name = N'(local)' 
  IF (@@ERROR <> 0 OR @ReturnCode <> 0) GOTO QuitWithRollback 

END
COMMIT TRANSACTION          
GOTO   EndSave              
QuitWithRollback:
  IF (@@TRANCOUNT > 0) ROLLBACK TRANSACTION 
EndSave: 


GO
