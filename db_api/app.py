from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

# Initialize Flask app
app = Flask(__name__)

# PostgreSQL database configuration
app.config['SQLALCHEMY_DATABASE_URI'] = 'postgresql://postgres:postgres@localhost/postgres'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

# Initialize SQLAlchemy
db = SQLAlchemy(app)

# Define a model for the database
class SensorData(db.Model):
    timestamp = db.Column(db.DateTime, default=datetime.utcnow, nullable=False, primary_key=True)  # Automatically calculated
    proximity_distance_cm = db.Column(db.Float, nullable=True)
    intruder_detected = db.Column(db.Boolean, nullable=True)
    force_detected = db.Column(db.Boolean, nullable=True)
    soft_alarm_triggered = db.Column(db.Boolean, nullable=True)
    hard_alarm_triggered = db.Column(db.Boolean, nullable=True)
    notification_type = db.Column(db.Text, nullable=True)
    notification_sent = db.Column(db.Boolean, nullable=True)
    event_type = db.Column(db.Text, nullable=True)

    def __repr__(self):
        return f"<Data timestamp={self.timestamp}"

# Create database tables
with app.app_context():
    db.create_all()

# Define the GET endpoint
@app.route('/save-sensor-data', methods=['GET'])
def save_data():
    # Extract query parameters
    proximity_distance_cm = request.args.get('proximity_distance_cm', type=int)
    intruder_detected = request.args.get('intruder_detected', type=lambda x: x.lower() == 'true')
    force_detected = request.args.get('force_detected', type=lambda x: x.lower() == 'true')
    soft_alarm_triggered = request.args.get('soft_alarm_triggered', type=lambda x: x.lower() == 'true')
    hard_alarm_triggered = request.args.get('hard_alarm_triggered', type=lambda x: x.lower() == 'true')
    notification_sent = request.args.get('notification_sent', type=lambda x: x.lower() == 'true')
    notification_type = request.args.get('notification_type')
    event_type = request.args.get('event_type')

    # Save to the database
    new_data = SensorData(
        proximity_distance_cm=proximity_distance_cm,
        intruder_detected=intruder_detected,
        force_detected=force_detected,
        soft_alarm_triggered=soft_alarm_triggered,
        hard_alarm_triggered=hard_alarm_triggered,
        notification_sent=notification_sent,
        notification_type=notification_type,
        event_type=event_type,
    )
    db.session.add(new_data)
    db.session.commit()

    return jsonify({'message': 'Data saved successfully'})

# Run the Flask app
if __name__ == '__main__':
    app.run(debug=True)
