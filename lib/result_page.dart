import 'package:flutter/material.dart';
import 'main.dart'; // Import the file where `SettingsItem` is defined.

class ResultPage extends StatelessWidget {
  final List<SettingsItem> settingsData;

  const ResultPage({Key? key, required this.settingsData}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Selected Settings'),
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text(
              'Your Selected Settings:',
              style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 16),
            Expanded(
              child: ListView.builder(
                itemCount: settingsData.length,
                itemBuilder: (context, index) {
                  final item = settingsData[index];
                  String value = item.isTextField
                      ? (item.textController?.text.isNotEmpty == true
                      ? item.textController!.text
                      : 'No value entered')
                      : (item.selectedOption ?? 'No option selected');

                  return ListTile(
                    title: Text(item.headerValue),
                    subtitle: Text(value),
                  );
                },
              ),
            ),
            const SizedBox(height: 16),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                _buildButton(context, 'OK', () => Navigator.pop(context)),
                _buildButton(context, 'Change', () => Navigator.pop(context)),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildButton(BuildContext context, String label, VoidCallback onPressed) {
    return ElevatedButton(
      onPressed: onPressed,
      child: Text(label),
    );
  }
}