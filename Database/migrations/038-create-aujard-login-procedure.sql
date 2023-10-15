CREATE OR ALTER PROCEDURE [dbo].[AUJARD_LOGIN]
@AccountID	varchar(21),
@Password	varchar(13),
@nRet		smallint	OUTPUT
AS
DECLARE @Nation tinyint, @CharNum smallint
SET @Nation = 0
SET @CharNum = 0
DECLARE @pwd varchar(13)
SET @pwd = null
SELECT @pwd = strPasswd FROM [dbo].[TB_USER] WHERE strAccountID = @AccountID  and  idays>0
IF @pwd IS null
BEGIN
	--SET @nRet = 0
             SET @nRet = 4
	RETURN
END
ELSE IF @pwd <> @Password
BEGIN
	--SET @nRet = 0
             SET @nRet = 3
	RETURN
END
SELECT @Nation = bNation, @CharNum = bCharNum FROM ACCOUNT_CHAR WHERE strAccountID = @AccountID
IF @@ROWCOUNT = 0
BEGIN
SET @nRet = 1
RETURN
END
ELSE IF @CharNum = 0
BEGIN
 SET @nRet = 1 -- Nation Zero (@Nation+1) 0+1?
 RETURN
END
ELSE IF (@CharNum <> 0) AND (@Nation = 1)
BEGIN
 SET @nRet = 2  -- Nation Karus (@Nation+1) 1+1
 RETURN
END
ELSE IF (@CharNum <> 0) AND (@Nation = 2)
BEGIN
 SET @nRet = 3  -- Nation Human (@Nation+1) 2+1
 RETURN
END

