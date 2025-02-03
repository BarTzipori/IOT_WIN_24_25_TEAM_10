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
  String _esp32Url = "http://172.20.10.10";
  String _connectionStatus = "Not Connected";

  final Map<String, TextEditingController> _controllers = {};

  @override
  void initState() {
    super.initState();
    _data = generateItems();
    for (var item in _data) {
      if (item.isTextField) {
        _controllers[item.headerValue] = TextEditingController();
        item.textController = _controllers[item.headerValue];
      }
    }

    _loadSavedSettings();
    _fetchLocalIP();
  }

  Future<void> _loadSavedSettings() async {
    final savedSettings = await _fetchSettingsFromFirebase();
    if (savedSettings != null) {
      _populateSettings(savedSettings);
    }
  }

  void _fetchLocalIP() async {
    try {
      DataSnapshot snapshot = await _databaseRef.child('System_Settings/localIP').get();
      if (snapshot.value != null) {
        setState(() {
          _esp32Url = "http://${snapshot.value}";
        });
        _connectionStatus = "Connected to $_esp32Url";
      }
    } catch (e) {
      setState(() {
        _connectionStatus = "Failed to fetch local IP";
      });
      print('Error fetching local IP: $e');
    }
  }


  @override
  void dispose() {
    _controllers.forEach((_, controller) => controller.dispose());
    super.dispose();
  }

  List<String> validateSettings() {
    List<String> errors = [];

    // Validate basic selections
    String? mode = _data.firstWhere((item) => item.headerValue == '1. Mode').selectedOption;
    String? alertMethod = _data.firstWhere((item) => item.headerValue == '2. Alert Method').selectedOption;
    String? alert1Status = _data.firstWhere((item) => item.headerValue == '3. Alert 1').selectedOption;
    String? alert2Status = _data.firstWhere((item) => item.headerValue == '4. Alert 2').selectedOption;
    String? alert3Status = _data.firstWhere((item) => item.headerValue == '5. Alert 3').selectedOption;

    // Check required selections
    if (mode == null) {
      errors.add('Mode must be selected');
    }
    if (alertMethod == null) {
      errors.add('Alert Method must be selected');
    }
    if (alert1Status == null) {
      errors.add('Alert 1 status must be selected');
    }

    // Validate Time to Impact settings
    if (alertMethod == 'Time to Impact') {
      double? timing1;
      double? timing2;
      double? timing3;

      if (alert1Status == 'Enable') {
        var timing1Item = _data.firstWhere((item) => item.headerValue == '6. Alert 1 Timing');
        timing1 = double.tryParse(timing1Item.selectedOption ?? '');
        if (timing1 == null) {
          errors.add('Alert 1 timing must be set when Alert 1 is enabled');
        }
      }

      if (alert2Status == 'Enable') {
        var timing2Item = _data.firstWhere((item) => item.headerValue == '7. Alert 2 Timing');
        timing2 = double.tryParse(timing2Item.selectedOption ?? '');
        if (timing2 == null) {
          errors.add('Alert 2 timing must be set when Alert 2 is enabled');
        }
      }

      if (alert3Status == 'Enable') {
        var timing3Item = _data.firstWhere((item) => item.headerValue == '8. Alert 3 Timing');
        timing3 = double.tryParse(timing3Item.selectedOption ?? '');
        if (timing3 == null) {
          errors.add('Alert 3 timing must be set when Alert 3 is enabled');
        }
      }

      // Validate timing order
      if (timing1 != null && timing2 != null && timing1 <= timing2) {
        errors.add('Alert 1 timing (${timing1}s) must be greater than Alert 2 timing (${timing2}s)');
      }
      if (timing2 != null && timing3 != null && timing2 <= timing3) {
        errors.add('Alert 2 timing (${timing2}s) must be greater than Alert 3 timing (${timing3}s)');
      }
      if (timing1 != null && timing3 != null && timing1 <= timing3) {
        errors.add('Alert 1 timing (${timing1}s) must be greater than Alert 3 timing (${timing3}s)');
      }
    }

    // Validate Distance settings
    if (alertMethod == 'Distance') {
      double? distance1;
      double? distance2;
      double? distance3;

      if (alert1Status == 'Enable') {
        distance1 = double.tryParse(_controllers['9. Alert 1 Distance']?.text ?? '');
        if (distance1 == null) {
          errors.add('Alert 1 distance must be set when Alert 1 is enabled');
        } else if (distance1 < 20 || distance1 > 250) {
          errors.add('Alert 1 distance must be between 20cm and 250cm');
        }
      }

      if (alert2Status == 'Enable') {
        distance2 = double.tryParse(_controllers['10. Alert 2 Distance']?.text ?? '');
        if (distance2 == null) {
          errors.add('Alert 2 distance must be set when Alert 2 is enabled');
        } else if (distance2 < 20 || distance2 > 250) {
          errors.add('Alert 2 distance must be between 20cm and 250cm');
        }
      }

      if (alert3Status == 'Enable') {
        distance3 = double.tryParse(_controllers['11. Alert 3 Distance']?.text ?? '');
        if (distance3 == null) {
          errors.add('Alert 3 distance must be set when Alert 3 is enabled');
        } else if (distance3 < 20 || distance3 > 250) {
          errors.add('Alert 3 distance must be between 20cm and 250cm');
        }
      }

      // Validate distance order
      if (distance1 != null && distance2 != null && distance1 <= distance2) {
        errors.add('Alert 1 distance (${distance1}cm) must be greater than Alert 2 distance (${distance2}cm)');
      }
      if (distance2 != null && distance3 != null && distance2 <= distance3) {
        errors.add('Alert 2 distance (${distance2}cm) must be greater than Alert 3 distance (${distance3}cm)');
      }
      if (distance1 != null && distance3 != null && distance1 <= distance3) {
        errors.add('Alert 1 distance (${distance1}cm) must be greater than Alert 3 distance (${distance3}cm)');
      }
    }

    // Validate heights and other numeric fields
    _validateRequiredFields(errors);

    // Validate numeric ranges for all applicable fields
    for (var item in _data) {
      _validateNumericRange(errors, item);
    }

    return errors;
  }

  void _validateRequiredFields(List<String> errors) {
    // Validate User Height
    String? userHeightText = _controllers['18. User Height']?.text;
    if (userHeightText == null || userHeightText.isEmpty) {
      errors.add('User Height is required');
    } else {
      double? userHeight = double.tryParse(userHeightText);
      if (userHeight == null) {
        errors.add('User Height must be a valid number');
      } else if (userHeight < 100 || userHeight > 250) {  // Assuming reasonable height range
        errors.add('User Height must be between 100cm and 250cm');
      }
    }

    // Validate System Height
    String? systemHeightText = _controllers['19. System Height']?.text;
    if (systemHeightText == null || systemHeightText.isEmpty) {
      errors.add('System Height is required');
    } else {
      double? systemHeight = double.tryParse(systemHeightText);
      if (systemHeight == null) {
        errors.add('System Height must be a valid number');
      } else if (systemHeight < 0 || systemHeight > 200) {  // Assuming reasonable range
        errors.add('System Height must be between 0cm and 200cm');
      }
    }

    // Validate Minimal Height
    String? minHeightText = _controllers['21. Minimal Height']?.text;
    if (minHeightText != null && minHeightText.isNotEmpty) {
      double? minHeight = double.tryParse(minHeightText);
      double? userHeight = double.tryParse(_controllers['18. User Height']?.text ?? '');

      if (minHeight != null && userHeight != null) {
        if (minHeight > userHeight) {
          errors.add('Minimal Height cannot be greater than User Height');
        }
        if (userHeight - minHeight < 20) {
          errors.add('Difference between User Height and Minimal Height must be at least 20cm');
        }
      }
    }

    // Validate Head Safety Margin
    String? safetyMarginText = _controllers['22. Head Safety Margin']?.text;
    if (safetyMarginText != null && safetyMarginText.isNotEmpty) {
      double? safetyMargin = double.tryParse(safetyMarginText);
      double? userHeight = double.tryParse(_controllers['18. User Height']?.text ?? '');
      double? systemHeight = double.tryParse(_controllers['19. System Height']?.text ?? '');

      if (safetyMargin != null && userHeight != null && systemHeight != null) {
        double effectiveHeight = userHeight - systemHeight - safetyMargin;
        if (effectiveHeight < 0) {
          errors.add('Head Safety Margin is too large for current User and System Heights');
        }
      }
    }

    // Validate Volume Sound if mode includes sound
    String? mode = _data.firstWhere((item) => item.headerValue == '1. Mode').selectedOption;
    if (mode != 'Vibration') {  // If mode is 'Sound' or 'Both'
      String? volume = _data.firstWhere((item) => item.headerValue == '20. Volume Sound').selectedOption;
      if (volume == null) {
        errors.add('Volume Sound must be selected when using sound alerts');
      }
    }
  }

  void _validateNumericRange(List<String> errors, SettingsItem item) {
    if (item.isNumericRange && item.textController != null) {
      String value = item.textController!.text.trim();
      if (value.isNotEmpty) {
        double? numValue = double.tryParse(value);
        if (numValue == null) {
          errors.add('${item.headerValue} must be a valid number');
        } else {
          if (item.minValue != null && numValue < item.minValue!) {
            errors.add('${item.headerValue} must be at least ${item.minValue}cm');
          }
          if (item.maxValue != null && numValue > item.maxValue!) {
            errors.add('${item.headerValue} must be at most ${item.maxValue}cm');
          }
        }
      }
    }
  }

  Future<void> _notifyESP32(Map<String, dynamic> settings) async {
    try {
      // Convert settings to JSON string
      final String jsonSettings = jsonEncode(settings);

      // Send settings to ESP32
      final response = await http.post(
        Uri.parse('$_esp32Url/settings'),
        headers: {'Content-Type': 'application/json'},
        body: jsonSettings,
      );

      if (response.statusCode != 200) {
        throw Exception('Failed to update ESP32 settings: ${response.statusCode}');
      }
    } catch (e) {
      print('Error updating ESP32: $e');
      throw e;  // Re-throw to be handled by the caller
    }
  }

  Future<void> _saveSettingsToDatabase() async {
    // First validate all settings
    List<String> errors = validateSettings();

    // If there are validation errors, show them and don't save
    if (errors.isNotEmpty) {
      if (!context.mounted) return;

      await showDialog(
        context: context,
        barrierDismissible: false,
        builder: (BuildContext context) {
          return AlertDialog(
            title: const Row(
              children: [
                Icon(Icons.error_outline, color: Colors.red),
                SizedBox(width: 8),
                Text('Validation Errors'),
              ],
            ),
            content: SingleChildScrollView(
              child: ListBody(
                children: errors.map((error) =>
                    Padding(
                      padding: const EdgeInsets.symmetric(vertical: 4),
                      child: Row(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text('• ', style: TextStyle(fontWeight: FontWeight.bold)),
                          Expanded(child: Text(error)),
                        ],
                      ),
                    )
                ).toList(),
              ),
            ),
            actions: <Widget>[
              TextButton(
                child: const Text('OK'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ],
          );
        },
      );
      return;
    }

    try {
      Map<String, dynamic> settingsData = {
        // Basic settings (strings)
        'mode': _data.firstWhere((item) => item.headerValue == '1. Mode',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'alertMethod': _data.firstWhere((item) => item.headerValue == '2. Alert Method',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'enableAlert1': _data.firstWhere((item) => item.headerValue == '3. Alert 1',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'enableAlert2': _data.firstWhere((item) => item.headerValue == '4. Alert 2',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'enableAlert3': _data.firstWhere((item) => item.headerValue == '5. Alert 3',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,

        // Alert timings (doubles)
        'alertTiming1': double.tryParse(_data.firstWhere((item) => item.headerValue == '6. Alert 1 Timing',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption ?? '0'),
        'alertTiming2': double.tryParse(_data.firstWhere((item) => item.headerValue == '7. Alert 2 Timing',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption ?? '0'),
        'alertTiming3': double.tryParse(_data.firstWhere((item) => item.headerValue == '8. Alert 3 Timing',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption ?? '0'),

        // Alert distances (doubles)
        'alertDistance1': _controllers['9. Alert 1 Distance']?.text != null && _controllers['9. Alert 1 Distance']!.text.isNotEmpty ?
        double.tryParse(_controllers['9. Alert 1 Distance']!.text) : null,
        'alertDistance2': _controllers['10. Alert 2 Distance']?.text != null && _controllers['10. Alert 2 Distance']!.text.isNotEmpty ?
        double.tryParse(_controllers['10. Alert 2 Distance']!.text) : null,
        'alertDistance3': _controllers['11. Alert 3 Distance']?.text != null && _controllers['11. Alert 3 Distance']!.text.isNotEmpty ?
        double.tryParse(_controllers['11. Alert 3 Distance']!.text) : null,

        // Vibration settings (strings)
        'alertVibration1': _data.firstWhere((item) => item.headerValue == '12. Alert 1 Vibration',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'alertVibration2': _data.firstWhere((item) => item.headerValue == '13. Alert 2 Vibration',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'alertVibration3': _data.firstWhere((item) => item.headerValue == '14. Alert 3 Vibration',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,

        // Sound settings (strings)
        'alertSound1': _data.firstWhere((item) => item.headerValue == '15. Alert 1 Sound',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'alertSound2': _data.firstWhere((item) => item.headerValue == '16. Alert 2 Sound',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,
        'alertSound3': _data.firstWhere((item) => item.headerValue == '17. Alert 3 Sound',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption,

        // Height and system settings (integers)
        'userHeight': _controllers['18. User Height']?.text != null && _controllers['18. User Height']!.text.isNotEmpty ?
        int.tryParse(_controllers['18. User Height']!.text) : null,
        'systemHeight': _controllers['19. System Height']?.text != null && _controllers['19. System Height']!.text.isNotEmpty ?
        int.tryParse(_controllers['19. System Height']!.text) : null,
        'volume': int.tryParse(_data.firstWhere((item) => item.headerValue == '20. Volume Sound',
            orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: [])).selectedOption ?? '1'),
        'minimalHeight': _controllers['21. Minimal Height']?.text != null && _controllers['21. Minimal Height']!.text.isNotEmpty ?
        int.tryParse(_controllers['21. Minimal Height']!.text) : null,
        'headSafetyMargin': _controllers['22. Head Safety Margin']?.text != null && _controllers['22. Head Safety Margin']!.text.isNotEmpty ?
        int.tryParse(_controllers['22. Head Safety Margin']!.text) : null,
      };

      // Remove null or empty values
      settingsData.removeWhere((key, value) =>
      value == null ||
          (value is String && value.isEmpty) ||
          value == ''
      );

      // First update Firebase
      await _databaseRef.child('System_Settings/settings').update(settingsData);

      // Then notify ESP32
      await _notifyESP32(settingsData);

      // Show success message
      if (!context.mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Settings saved successfully!'),
          backgroundColor: Colors.green,
          duration: Duration(seconds: 2),
        ),
      );
    } catch (e) {
      print('Error saving settings: $e');
      // Show error message
      if (!context.mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text('Error saving settings: $e'),
          backgroundColor: Colors.red,
          duration: const Duration(seconds: 4),
        ),
      );
    }
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
        expandedValue: 'Enter distance for Alert 1 [20cm - 250cm]',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
      ),
      SettingsItem(
        headerValue: '10. Alert 2 Distance',
        expandedValue: 'Enter distance for Alert 2 (20cm - 250cm)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '11. Alert 3 Distance',
        expandedValue: 'Enter distance for Alert 2 (20cm - 250cm)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
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
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
      ),
      SettingsItem(
        headerValue: '16. Alert 2 Sound',
        expandedValue: 'Choose sound type for Alert 2',
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '17. Alert 3 Sound',
        expandedValue: 'Choose sound type for Alert 3',
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
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
      SettingsItem(
        headerValue: '21. Minimal Height',
        expandedValue: 'Enter minimal height in centimeters',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 50,
        maxValue: 200,
      ),
      SettingsItem(
        headerValue: '22. Head Safety Margin',
        expandedValue: 'Enter head safety margin in centimeters',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 5,
        maxValue: 20,
      ),
    ];
  }

  Future<Map<String, dynamic>?> _fetchSettingsFromFirebase() async {
    try {
      final DataSnapshot snapshot = await _databaseRef.child('System_Settings/settings').get();
      if (snapshot.value != null) {
        return Map<String, dynamic>.from(snapshot.value as Map);
      }
    } catch (e) {
      print('Error fetching settings from Firebase: $e');
    }
    return null;
  }
  void _populateSettings(Map<String, dynamic> settings) {
    print('Received settings from Firebase: $settings'); // Debug print

    final Map<String, String> varNameToHeader = {
      'mode': '1. Mode',
      'alertMethod': '2. Alert Method',
      'enableAlert1': '3. Alert 1',
      'enableAlert2': '4. Alert 2',
      'enableAlert3': '5. Alert 3',
      'alertTiming1': '6. Alert 1 Timing',
      'alertTiming2': '7. Alert 2 Timing',
      'alertTiming3': '8. Alert 3 Timing',
      'alertDistance1': '9. Alert 1 Distance',
      'alertDistance2': '10. Alert 2 Distance',
      'alertDistance3': '11. Alert 3 Distance',
      'alertVibration1': '12. Alert 1 Vibration',
      'alertVibration2': '13. Alert 2 Vibration',
      'alertVibration3': '14. Alert 3 Vibration',
      'alertSound1': '15. Alert 1 Sound',
      'alertSound2': '16. Alert 2 Sound',
      'alertSound3': '17. Alert 3 Sound',
      'userHeight': '18. User Height',
      'systemHeight': '19. System Height',
      'volume': '20. Volume Sound',
      'minimalHeight': '21. Minimal Height',
      'headSafetyMargin': '22. Head Safety Margin'
    };

    settings.forEach((key, value) {
      String? headerValue = varNameToHeader[key];
      if (headerValue != null) {
        var item = _data.firstWhere(
              (element) => element.headerValue == headerValue,
          orElse: () => SettingsItem(headerValue: '', expandedValue: '', options: []),
        );

        if (item.headerValue.isNotEmpty) {
          if (item.isTextField) {
            // For text fields, convert the number to string for display
            item.textController?.text = value.toString();
          } else {
            if (headerValue.contains('Timing')) {
              // For timing options, handle as double
              setState(() {
                item.selectedOption = value.toDouble().toString();
              });
            } else {
              setState(() {
                item.selectedOption = value.toString();
              });
            }
          }
        }
      }
    });
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

    // Get selected alert method
    String? alertMethod = _data.firstWhere(
          (element) => element.headerValue == '2. Alert Method',
      orElse: () => _data[0],
    ).selectedOption;

    // Hide timing panels when Distance is selected
    if (alertMethod == 'Distance') {
      if (item.headerValue == '6. Alert 1 Timing' ||
          item.headerValue == '7. Alert 2 Timing' ||
          item.headerValue == '8. Alert 3 Timing') {
        return false;
      }
    }

    // Hide distance panels when Time to Impact is selected
    if (alertMethod == 'Time to Impact') {
      if (item.headerValue == '9. Alert 1 Distance' ||
          item.headerValue == '10. Alert 2 Distance' ||
          item.headerValue == '11. Alert 3 Distance') {
        return false;
      }
    }

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
    final visibleItems = _data.where((item) => _shouldShowItem(item)).toList();

    return Scaffold(
      appBar: AppBar(
        title: const Text(
          'System Settings',
          style: TextStyle(fontSize: 20),
          overflow: TextOverflow.ellipsis,
        ),
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
                  final originalIndex = _data.indexOf(visibleItems[index]);
                  setState(() {
                    _data[originalIndex].isExpanded = !_data[originalIndex].isExpanded;
                  });
                },
                children: visibleItems.map<ExpansionPanel>((SettingsItem item) {
                  return ExpansionPanel(
                    headerBuilder: (BuildContext context, bool isExpanded) {
                      return ListTile(
                        title: Text(
                          item.headerValue,
                          style: const TextStyle(
                            fontWeight: FontWeight.bold,
                            fontSize: 16,
                          ),
                          overflow: TextOverflow.ellipsis,
                        ),
                        subtitle: Text(
                          item.expandedValue,
                          style: const TextStyle(fontSize: 14),
                          overflow: TextOverflow.ellipsis,
                        ),
                      );
                    },
                    body: item.isTextField
                        ? Padding(
                      padding: const EdgeInsets.symmetric(
                        horizontal: 16.0,
                        vertical: 8.0,
                      ),
                      child: LayoutBuilder(
                        builder: (context, constraints) {
                          return Row(
                            children: [
                              Expanded(
                                child: TextField(
                                  controller: item.textController,
                                  decoration: InputDecoration(
                                    labelText: item.isNumericRange
                                        ? 'Value (${item.minValue}-${item.maxValue}cm)'
                                        : 'Value',
                                    border: const OutlineInputBorder(),
                                    isDense: true,
                                    contentPadding: const EdgeInsets.symmetric(
                                      horizontal: 12,
                                      vertical: 8,
                                    ),
                                    suffixText: item.headerValue.toLowerCase().contains('height')
                                        ? 'cm'
                                        : (item.isNumericRange ? 'cm' : null),
                                  ),
                                  style: const TextStyle(fontSize: 14),
                                  keyboardType: const TextInputType.numberWithOptions(decimal: true),
                                ),
                              ),
                            ],
                          );
                        },
                      ),
                    )
                        : LayoutBuilder(
                      builder: (context, constraints) {
                        return Column(
                          children: item.options.map((String option) {
                            return Container(
                              constraints: BoxConstraints(
                                maxWidth: constraints.maxWidth,
                              ),
                              child: Row(
                                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                children: [
                                  Expanded(
                                    child: RadioListTile<String>(
                                      title: Text(
                                        option,
                                        style: const TextStyle(fontSize: 14),
                                        overflow: TextOverflow.ellipsis,
                                      ),
                                      value: option,
                                      groupValue: item.selectedOption,
                                      onChanged: (String? value) {
                                        setState(() {
                                          item.selectedOption = value;
                                        });
                                      },
                                      dense: true,
                                      contentPadding: const EdgeInsets.symmetric(
                                        horizontal: 8.0,
                                      ),
                                    ),
                                  ),
                                  if (item.headerValue.contains('Sound') ||
                                      item.headerValue.contains('Vibration'))
                                    Padding(
                                      padding: const EdgeInsets.only(right: 8.0),
                                      child: SizedBox(
                                        width: 70,
                                        height: 30,
                                        child: ElevatedButton(
                                          onPressed: () {
                                            if (item.headerValue.contains('Sound')) {
                                              _makeSound(option);
                                            } else {
                                              _makeVibration(option);
                                            }
                                          },
                                          style: ElevatedButton.styleFrom(
                                            padding: const EdgeInsets.symmetric(
                                              horizontal: 8.0,
                                            ),
                                          ),
                                          child: const Text(
                                            'Play',
                                            style: TextStyle(fontSize: 12),
                                          ),
                                        ),
                                      ),
                                    ),
                                ],
                              ),
                            );
                          }).toList(),
                        );
                      },
                    ),
                    isExpanded: item.isExpanded,
                  );
                }).toList(),
              ),
              const SizedBox(height: 16),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  ElevatedButton(
                    onPressed: _testConnection,
                    child: const Text(
                      'Test Connection',
                      style: TextStyle(fontSize: 14),
                    ),
                  ),
                  ElevatedButton(
                    onPressed: _saveSettingsToDatabase,
                    child: const Text(
                      'Save Settings',
                      style: TextStyle(fontSize: 14),
                    ),
                  ),
                ],
              ),
              Padding(
                padding: const EdgeInsets.all(8.0),
                child: Text(
                  'Connection Status: $_connectionStatus',
                  style: const TextStyle(fontSize: 14),
                  overflow: TextOverflow.ellipsis,
                ),
              ),
              const SizedBox(height: 32),
            ],
          ),
        ),
      ),
    );
  }
}
