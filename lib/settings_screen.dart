import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

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
  final DatabaseReference _databaseRef = FirebaseDatabase.instance.ref();

  @override
  void initState() {
    super.initState();
    _data = generateItems();
  }

  @override
  void dispose() {
    _heightController.dispose();
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
        options: ['Vibration 1', 'Vibration 2', 'Vibration 3', 'None'],
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
    ];
  }

  Future<void> _saveSettingsToDatabase() async {
    try {
      final Map<String, dynamic> settingsData = {
        'soundOrVibration': _data[0].selectedOption,
        'soundType': _data[1].selectedOption,
        'vibrationType': _data[2].selectedOption,
        'notificationTiming': _data[3].selectedOption,
        'userHeight': _data[4].textController?.text,
      };

      await _databaseRef.child('System_Settings/settings').set(settingsData);

      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Settings saved successfully!')),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Failed to save settings: $e')),
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
                    body: Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: item.isTextField
                          ? TextField(
                        controller: item.textController,
                        decoration: const InputDecoration(
                          labelText: 'Height (m)',
                          hintText: 'Enter your height (e.g., 1.75)',
                          border: OutlineInputBorder(),
                          suffixText: 'm',
                        ),
                        keyboardType: TextInputType.numberWithOptions(decimal: true),
                      )
                          : Column(
                        children: item.options.map((String option) {
                          return RadioListTile<String>(
                            title: Text(option),
                            value: option,
                            groupValue: item.selectedOption,
                            onChanged: (String? value) {
                              setState(() {
                                item.selectedOption = value;
                              });
                            },
                          );
                        }).toList(),
                      ),
                    ),
                    isExpanded: item.isExpanded,
                  );
                }).toList(),
              ),
              const SizedBox(height: 24),
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
