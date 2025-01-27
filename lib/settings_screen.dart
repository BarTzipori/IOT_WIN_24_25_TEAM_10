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
    this.dependsOn,
    this.isNumericRange = false,
    this.minValue,
    this.maxValue,
  });

  List<String> options;
  String headerValue;
  String expandedValue;
  bool isExpanded;
  String? selectedOption;
  bool isTextField;
  TextEditingController? textController;
  String? dependsOn;
  bool isNumericRange;
  double? minValue;
  double? maxValue;
}

class SettingsQuestionnaire extends StatefulWidget {
  const SettingsQuestionnaire({super.key});

  @override
  State<SettingsQuestionnaire> createState() => _SettingsQuestionnaireState();
}

class _SettingsQuestionnaireState extends State<SettingsQuestionnaire> {
  late List<SettingsItem> _data;
  final DatabaseReference _databaseRef = FirebaseDatabase.instance.ref();
  final String _esp32Url = "http://172.20.10.3";
  String _connectionStatus = "Not Connected";

  final Map<String, TextEditingController> _controllers = {};

  @override
  void initState() {
    super.initState();
    _data = generateItems();
    // Initialize controllers for text fields
    for (var item in _data) {
      if (item.isTextField) {
        _controllers[item.headerValue] = TextEditingController();
        item.textController = _controllers[item.headerValue];
      }
    }
  }

  @override
  void dispose() {
    _controllers.forEach((_, controller) => controller.dispose());
    super.dispose();
  }

  List<String> validateSettings() {
    List<String> errors = [];
    String? alertMethod = _data.firstWhere((item) => item.headerValue == '2. Alert Method').selectedOption;

    if (alertMethod == 'Time to Impact') {
      double? timing1 = double.tryParse(_data.firstWhere((item) => item.headerValue == '6. Alert 1 Timing').selectedOption ?? '');
      double? timing2 = double.tryParse(_data.firstWhere((item) => item.headerValue == '7. Alert 2 Timing').selectedOption ?? '');
      double? timing3 = double.tryParse(_data.firstWhere((item) => item.headerValue == '8. Alert 3 Timing').selectedOption ?? '');

      if (timing1 != null && timing2 != null) {
        // Check if they follow the required order
        if (!( timing2 < timing1 )) {
          errors.add('Alert timings must follow the order: Alert 2 < Alert 1');
        }
      }

      // Check if all timings are set
      if (timing1 != null && timing2 != null && timing3 != null) {
        // Check if they follow the required order
        if (!(timing3 < timing2 || timing2 < timing1 || timing3 < timing1)) {
          errors.add('Alert timings must follow the order: Alert 3 < Alert 2 < Alert 1');
        }
      }
    } else if (alertMethod == 'Distance') {
      double? distance1 = double.tryParse(_controllers['9. Alert 1 Distance']?.text ?? '');
      double? distance2 = double.tryParse(_controllers['10. Alert 2 Distance']?.text ?? '');
      double? distance3 = double.tryParse(_controllers['11. Alert 3 Distance']?.text ?? '');

      // Check range validation
      if (distance1 != null && (distance1 < 0.25 || distance1 > 2.5)) {
        errors.add('Alert 1 distance must be between [0.25m - 2.5m]');
      }

      if (distance2 != null && (distance2 < 0.25 || distance2 > 2.5)) {
        errors.add('Alert 2 distance must be between [0.25m - 2.5m]');
      }

      if (distance3 != null && (distance3 < 0.25 || distance3 > 2.5)) {
        errors.add('Alert 3 distance must be between [0.25m - 2.5m]');
      }

      if (distance1 != null && distance2 != null) {
        // Check if they follow the required order
        if (!( distance2 < distance1 )) {
          errors.add('Alert distances must follow the order: Alert 2 < Alert 1');
        }
      }

      // Check if all distances are set and within range
      if (distance1 != null && distance2 != null && distance3 != null) {
        // Check if they follow the required order
        if (!(distance3 < distance2 || distance2 < distance1 || distance3 < distance1 )) {
          errors.add('Alert distances must follow the order: Alert 3 < Alert 2 < Alert 1');
        }
      }
    }
    return errors;
  }

  List<double> _generateTimingOptions() {
    List<double> options = [];
    for (double i = 0.5; i <= 2.5; i += 0.1) {
      options.add(double.parse(i.toStringAsFixed(1)));
    }
    return options;
  }

  List<SettingsItem> generateItems() {
    List<double> timingOptions = _generateTimingOptions();

    return [
      SettingsItem(
        headerValue: '1. Mode',
        expandedValue: 'Choose the alert mode',
        options: ['Sound', 'Vibration', 'Both'],
      ),
      SettingsItem(
        headerValue: '2. Alert Method',
        expandedValue: 'Choose between time to impact or distance',
        options: ['Time to Impact', 'Distance'],
      ),
      SettingsItem(
        headerValue: '3. Alert 1',
        expandedValue: 'Enable or disable Alert 1',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '4. Alert 2',
        expandedValue: 'Enable or disable Alert 2',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '5. Alert 3',
        expandedValue: 'Enable or disable Alert 3',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '6. Alert 1 Timing',
        expandedValue: 'Set timing for Alert 1',
        options: timingOptions.map((e) => e.toString()).toList(),
      ),
      SettingsItem(
        headerValue: '7. Alert 2 Timing',
        expandedValue: 'Set timing for Alert 2',
        options: timingOptions.map((e) => e.toString()).toList(),
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '8. Alert 3 Timing',
        expandedValue: 'Set timing for Alert 3',
        options: timingOptions.map((e) => e.toString()).toList(),
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '9. Alert 1 Distance',
        expandedValue: 'Enter distance for Alert 1 (0.25m - 2.5m)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 0.25,
        maxValue: 2.5,
      ),
      SettingsItem(
        headerValue: '10. Alert 2 Distance',
        expandedValue: 'Enter distance for Alert 2 (0.25m - 2.5m)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 0.25,
        maxValue: 2.5,
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '11. Alert 3 Distance',
        expandedValue: 'Enter distance for Alert 3 (0.25m - 2.5m)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 0.25,
        maxValue: 2.5,
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '12. Alert 1 Vibration',
        expandedValue: 'Choose vibration type for Alert 1',
        options: ['Short', 'Long', 'Double', 'Pulse'],
      ),
      SettingsItem(
        headerValue: '13. Alert 2 Vibration',
        expandedValue: 'Choose vibration type for Alert 2',
        options: ['Short', 'Long', 'Double', 'Pulse'],
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '14. Alert 3 Vibration',
        expandedValue: 'Choose vibration type for Alert 3',
        options: ['Short', 'Long', 'Double', 'Pulse'],
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '15. Alert 1 Sound',
        expandedValue: 'Choose sound type for Alert 1',
        options: List.generate(8, (index) => 'Sound ${index + 1}'),
      ),
      SettingsItem(
        headerValue: '16. Alert 2 Sound',
        expandedValue: 'Choose sound type for Alert 2',
        options: List.generate(8, (index) => 'Sound ${index + 1}'),
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '17. Alert 3 Sound',
        expandedValue: 'Choose sound type for Alert 3',
        options: List.generate(8, (index) => 'Sound ${index + 1}'),
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '18. User Height',
        expandedValue: 'Enter user height in centimeters',
        options: [],
        isTextField: true,
      ),
      SettingsItem(
        headerValue: '19. System Height',
        expandedValue: 'Enter system height in centimeters',
        options: [],
        isTextField: true,
      ),
      SettingsItem(
        headerValue: '20. Volume Sound',
        expandedValue: 'Set sound volume (1-5)',
        options: List.generate(5, (index) => (index + 1).toString()),
      ),
    ];
  }

  Future<void> _saveSettingsToDatabase() async {
    // Collect all validation errors
    List<String> errors = validateSettings();

    // If there are any errors, display them and don't save
    if (errors.isNotEmpty) {
      String errorMessage = 'Cannot save due to the following errors:\n' +
          errors.map((e) => '• $e').join('\n');

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(errorMessage),
          backgroundColor: Colors.red,
          duration: const Duration(seconds: 5),
        ),
      );
      return;
    }

    try {
      Map<String, dynamic> settingsData = {};

      final Map<String, String> headerToVarName = {
        '1. Mode': 'mode',
        '2. Alert Method': 'alertMethod',
        '3. Alert 1': 'enableAlert1',
        '4. Alert 2': 'enableAlert2',
        '5. Alert 3': 'enableAlert3',
        '6. Alert 1 Timing': 'alertTiming1',
        '7. Alert 2 Timing': 'alertTiming2',
        '8. Alert 3 Timing': 'alertTiming3',
        '9. Alert 1 Distance': 'alertDistance1',
        '10. Alert 2 Distance': 'alertDistance2',
        '11. Alert 3 Distance': 'alertDistance3',
        '12. Alert 1 Vibration': 'alertVibration1',
        '13. Alert 2 Vibration': 'alertVibration2',
        '14. Alert 3 Vibration': 'alertVibration3',
        '15. Alert 1 Sound': 'alertSound1',
        '16. Alert 2 Sound': 'alertSound2',
        '17. Alert 3 Sound': 'alertSound3',
        '18. User Height': 'userHeight',
        '19. System Height': 'systemHeight',
        '20. Volume Sound': 'volume'
      };

      // Define which fields should be treated as numbers
      final Set<String> numericFields = {
        'alertTiming1',
        'alertTiming2',
        'alertTiming3',
        'alertDistance1',
        'alertDistance2',
        'alertDistance3',
        'userHeight',
        'systemHeight',
        'volume'
      };

      for (var item in _data) {
        String? dbVarName = headerToVarName[item.headerValue];
        if (dbVarName != null) {
          if (item.isTextField) {
            String value = item.textController?.text.trim() ?? '';
            if (numericFields.contains(dbVarName) && value.isNotEmpty) {
              // Convert to double first for decimal numbers
              double? numValue = double.tryParse(value);
              if (numValue != null) {
                if (dbVarName == 'volume' ||
                    dbVarName == 'userHeight' ||
                    dbVarName == 'systemHeight') {
                  // Convert to integer for specific fields
                  settingsData[dbVarName] = numValue.round();
                } else {
                  // Keep as double for distances and timings
                  settingsData[dbVarName] = numValue;
                }
              }
            } else {
              settingsData[dbVarName] = value;
            }
          } else {
            String value = item.selectedOption ?? '';
            if (numericFields.contains(dbVarName) && value.isNotEmpty) {
              // Convert to double first for decimal numbers
              double? numValue = double.tryParse(value);
              if (numValue != null) {
                if (dbVarName == 'volume') {
                  // Convert to integer for volume
                  settingsData[dbVarName] = numValue.round();
                } else {
                  // Keep as double for distances and timings
                  settingsData[dbVarName] = numValue;
                }
              }
            } else {
              settingsData[dbVarName] = value;
            }
          }
        }
      }

      print('Settings Data to Save: $settingsData');
      settingsData.removeWhere((key, value) => value.toString().isEmpty);

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
      setState(() {
        _connectionStatus = response.statusCode == 200
            ? "Connected to ESP32"
            : "Connection failed: ${response.statusCode}";
      });
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

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(response.statusCode == 200
              ? 'Playing $sound'
              : 'Failed to play $sound. Status: ${response.statusCode}'),
        ),
      );
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

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(response.statusCode == 200
              ? 'Playing $vibration'
              : 'Failed to play $vibration. Status: ${response.statusCode}'),
        ),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error: $e')),
      );
    }
  }

  bool _shouldShowItem(SettingsItem item) {
    // First check if the item should be hidden based on mode selection
    String? selectedMode = _data.firstWhere(
          (element) => element.headerValue == '1. Mode',
      orElse: () => _data[0],
    ).selectedOption;

    // Handle vibration panels visibility
    if (item.headerValue.contains('Vibration')) {
      if (selectedMode == 'Sound') {
        return false;
      }
    }

    // Handle sound panels visibility
    if (item.headerValue.contains('Sound') &&
        !item.headerValue.contains('Volume')) {  // Exclude Volume Sound from this check
      if (selectedMode == 'Vibration') {
        return false;
      }
    }

    // Only show Volume Sound setting if mode includes Sound
    if (item.headerValue == '20. Volume Sound') {
      if (selectedMode == 'Vibration') {
        return false;
      }
    }

    // Then check dependencies as before
    if (item.dependsOn == null) return true;

    var dependsOnNumber = item.dependsOn?.split('.')[0].trim();
    final dependentItem = _data.firstWhere(
          (element) => element.headerValue.startsWith(dependsOnNumber ?? ''),
      orElse: () => _data[0],
    );

    return dependentItem.selectedOption == 'Enable';
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
                  if (!_shouldShowItem(item)) {
                    return ExpansionPanel(
                      headerBuilder: (_, __) => const SizedBox(),
                      body: const SizedBox(),
                      isExpanded: false,
                      canTapOnHeader: false,
                    );
                  }

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
                        ? Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: TextField(
                        controller: item.textController,
                        decoration: InputDecoration(
                          labelText: item.isNumericRange
                              ? 'Enter value (${item.minValue}-${item.maxValue}m)'
                              : 'Enter value',
                          border: const OutlineInputBorder(),
                          suffixText: item.headerValue.toLowerCase().contains('height')
                              ? 'cm'
                              : (item.isNumericRange ? 'm' : null),
                        ),
                        keyboardType: const TextInputType.numberWithOptions(decimal: true),
                        onChanged: (value) {},
                      ),
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
                            if (item.headerValue.contains('Sound') ||
                                item.headerValue.contains('Vibration'))
                              Padding(
                                padding: const EdgeInsets.only(right: 16.0),
                                child: ElevatedButton(
                                  onPressed: () {
                                    if (item.headerValue.contains('Sound')) {
                                      _makeSound(option);
                                    } else {
                                      _makeVibration(option);
                                    }
                                  },
                                  child: const Text('Play'),
                                ),
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
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: _saveSettingsToDatabase,
                child: const Text('Save Settings'),
              ),
              const SizedBox(height: 32),
            ],
          ),
        ),
      ),
    );
  }
}