% Read the soil moisture channel data from the past two weeks.
% Send an email and tell the user to add water if the value
% is in the lowest 10 %.

% Store the channel ID for the moisture sensor channel.
channelID = 2495536;

% Provide the ThingSpeak alerts API key.  All alerts API keys start with TAK.
alertApiKey = "TAKxJVySQyJKPk+fI+R";

% Set the address for the HTTTP call
alertUrl="https://api.thingspeak.com/alerts/send";

% webwrite uses weboptions to add required headers.  Alerts needs a ThingSpeak-Alerts-API-Key header.
options = weboptions("HeaderFields", ["ThingSpeak-Alerts-API-Key", alertApiKey ]);

% Set the email subject.
alertSubject = sprintf("Temperature action needed");

% Read the recent data.
tempData = thingSpeakRead(channelID,'NumPoints',1,'Fields',1);

% Check to make sure the data was read correctly from the channel.
if isempty(tempData)
    alertBody = 'No data read';
else
    lastValue = tempData(end);
    dangerLevelHigh = 26;
    dangerLevelLow = 15;

    % Set the outgoing message
    if lastValue > dangerLevelHigh
        alertBody = sprintf('Temperature is in danger level of %0.2f. Close any open spaces that let the air in', lastValue);
    elseif (lastValue<dangerLevelLow)
        alertBody = sprintf('Temperature is in danger level of %0.2f. Open all closed spaces that let the air in', lastValue);
    end
end

 % Catch errors so the MATLAB code does not disable a TimeControl if it fails
try
    webwrite(alertUrl , "body", alertBody, "subject", alertSubject, options);
catch someException
    fprintf("Failed to send alert: %s\n", someException.message);
end
