import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

class SettingsItem {
  SettingsItem({
    required this.expandedValue,
    required this.headerValue,
    required this.options,
    this.isExpanded = false,
    this.selectedOption,
    this.isTextField = false,
    this.textController,
  });

  List<String> options;
  String headerValue;
  String expandedValue;
  bool isExpanded;
  String? selectedOption;
  bool isTextField;
  TextEditingController? textController;
}

class SettingsQuestionnaire extends StatefulWidget {
  const SettingsQuestionnaire({super.key});

  @override
  State<SettingsQuestionnaire> createState() => _SettingsQuestionnaireState();
}

class _SettingsQuestionnaireState extends State<SettingsQuestionnaire> {
  late List<SettingsItem> _data;
  final TextEditingController _heightController = TextEditingController();
  final TextEditingController _systemHeightController = TextEditingController();
  final DatabaseReference _databaseRef = FirebaseDatabase.instance.ref();
  final String _esp32Url = "http://172.20.10.10";
  String _connectionStatus = "Not Connected";

  @override
  void initState() {
    super.initState();
    _data = generateItems();
  }

  @override
  void dispose() {
    _heightController.dispose();
    _systemHeightController.dispose();
    super.dispose();
  }

  List<SettingsItem> generateItems() {
    return [
      SettingsItem(
        headerValue: '1. Sound or Vibration',
        expandedValue: 'Choose between sound or vibration alerts',
        options: ['Sound', 'Vibration', 'Both'],
      ),
      SettingsItem(
        headerValue: '2. Sound Type',
        expandedValue: 'Select the type of sound alert',
        options: ['Sound 1', 'Sound 2', 'Sound 3', 'None'],
      ),
      SettingsItem(
        headerValue: '3. Vibration Type',
        expandedValue: 'Select the type of vibration alert',
        options: ['Short', 'Long', 'Double', 'Pulse', 'None'],
      ),
      SettingsItem(
        headerValue: '4. Notification Timing',
        expandedValue: 'When to notify about potential obstacles (in seconds before impact)',
        options: ['1 second', '2 seconds', '3 seconds'],
      ),
      SettingsItem(
        headerValue: '5. User Height',
        expandedValue: 'Enter your height in meters',
        options: [],
        isTextField: true,
        textController: _heightController,
      ),
      SettingsItem(
        headerValue: '6. System Height',
        expandedValue: 'Enter the system height in meters',
        options: [],
        isTextField: true,
        textController: _systemHeightController,
      ),
    ];
  }

  Future<void> _saveSettingsToDatabase() async {
    try {
      // Prepare settings data
      final Map<String, dynamic> settingsData = {
        'soundOrVibration': _data[0].selectedOption ?? '',
        'soundType': _data[1].selectedOption ?? '',
        'vibrationType': _data[2].selectedOption ?? '',
        'notificationTiming': _data[3].selectedOption ?? '',
        'userHeight': _data[4].textController?.text.trim() ?? '',
        'systemHeight': _data[5].textController?.text.trim() ?? '',
      };

      // Log data for debugging
      print('Settings Data to Save: $settingsData');

      // Remove empty values (optional)
      settingsData.removeWhere((key, value) => value.isEmpty);

      // Save to Firebase
      await _databaseRef.child('System_Settings/settings').update(settingsData);

      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Settings saved successfully!')),
      );
    } catch (e) {
      print('Error saving settings to Firebase: $e');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error saving settings: $e')),
      );
    }
  }

  Future<void> _testConnection() async {
    try {
      final response = await http.get(Uri.parse(_esp32Url));
      if (response.statusCode == 200) {
        setState(() {
          _connectionStatus = "Connected to ESP32";
        });
      } else {
        setState(() {
          _connectionStatus = "Connection failed: ${response.statusCode}";
        });
      }
    } catch (e) {
      setState(() {
        _connectionStatus = "Connection error: $e";
      });
    }
  }

  Future<void> _makeSound(String sound) async {
    try {
      final url = Uri.parse("$_esp32Url/play_sound=${Uri.encodeComponent(sound)}");
      final response = await http.get(url);

      if (response.statusCode == 200) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Playing $sound')),
        );
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to play $sound. Status: ${response.statusCode}')),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error: $e')),
      );
    }
  }

  Future<void> _makeVibration(String vibration) async {
    try {
      final url = Uri.parse("$_esp32Url/play_vibration=${Uri.encodeComponent(vibration)}");
      final response = await http.get(url);

      if (response.statusCode == 200) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Playing $vibration')),
        );
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to play $vibration. Status: ${response.statusCode}')),
        );
      }
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error: $e')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('System Settings'),
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
      ),
      body: SingleChildScrollView(
        child: Container(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            children: [
              ExpansionPanelList(
                expandedHeaderPadding: const EdgeInsets.all(0),
                expansionCallback: (int index, bool isExpanded) {
                  setState(() {
                    _data[index].isExpanded = !_data[index].isExpanded;
                  });
                },
                children: _data.map<ExpansionPanel>((SettingsItem item) {
                  return ExpansionPanel(
                    headerBuilder: (BuildContext context, bool isExpanded) {
                      return ListTile(
                        title: Text(
                          item.headerValue,
                          style: const TextStyle(fontWeight: FontWeight.bold),
                        ),
                        subtitle: Text(item.expandedValue),
                      );
                    },
                    body: item.isTextField
                        ? TextField(
                      controller: item.textController,
                      decoration: InputDecoration(
                        labelText: item.headerValue.contains('Height')
                            ? 'Height (m)'
                            : 'Enter Value',
                        hintText: item.headerValue.contains('Height')
                            ? 'Enter height (e.g., 1.75)'
                            : '',
                        border: const OutlineInputBorder(),
                        suffixText: item.headerValue.contains('Height')
                            ? 'm'
                            : null,
                      ),
                      keyboardType: const TextInputType.numberWithOptions(decimal: true),
                    )
                        : Column(
                      children: item.options.map((String option) {
                        return Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            Expanded(
                              child: RadioListTile<String>(
                                title: Text(option),
                                value: option,
                                groupValue: item.selectedOption,
                                onChanged: (String? value) {
                                  setState(() {
                                    item.selectedOption = value;
                                  });
                                },
                              ),
                            ),
                            if ((item.headerValue == '2. Sound Type' && option != 'None') ||
                                (item.headerValue == '3. Vibration Type' && option != 'None'))
                              ElevatedButton(
                                onPressed: () {
                                  if (item.headerValue == '2. Sound Type') {
                                    _makeSound(option);
                                  } else {
                                    _makeVibration(option);
                                  }
                                },
                                child: const Text('Play'),
                              ),
                          ],
                        );
                      }).toList(),
                    ),
                    isExpanded: item.isExpanded,
                  );
                }).toList(),
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: _testConnection,
                child: const Text('Test Connection'),
              ),
              Text('Connection Status: $_connectionStatus'),
              ElevatedButton(
                onPressed: _saveSettingsToDatabase,
                child: const Text('Save'),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
