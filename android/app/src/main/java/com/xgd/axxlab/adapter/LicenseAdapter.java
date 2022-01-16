package com.xgd.axxlab.adapter;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.xgd.axxlab.R;
import com.xgd.axxlab.data.LicenseData;

import java.util.List;

public class LicenseAdapter extends RecyclerView.Adapter<LicenseAdapter.ViewHolder> {
    private List<LicenseData> mLicenseList;

    public LicenseAdapter(List<LicenseData> licenseDataList) {
        mLicenseList = licenseDataList;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_license, parent, false);
        final ViewHolder holder = new ViewHolder(view);
//        holder.view.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                int position = holder.getAdapterPosition();
//                LicenseData licenseData = mLicenseList.get(position);
//                Toast.makeText(v.getContext(),
//                        "you clicked view " + licenseData.getName(),
//                        Toast.LENGTH_SHORT).show();
//            }
//        });
//        holder.image.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                int position = holder.getAdapterPosition();
//                LicenseData licenseData = mLicenseList.get(position);
//                Toast.makeText(v.getContext(),
//                        "you clicked image " + licenseData.getName(),
//                        Toast.LENGTH_SHORT).show();
//            }
//        });
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        LicenseData licenseData = mLicenseList.get(position);
        holder.name.setText(licenseData.getName());
        holder.image.setImageResource(licenseData.getImageId());
        holder.detail.setText(licenseData.getDetail());
    }

    @Override
    public int getItemCount() {
        return mLicenseList.size();
    }

    static class ViewHolder extends RecyclerView.ViewHolder {
        View view;
        ImageView image;
        TextView name;
        TextView detail;

        public ViewHolder(View view) {
            super(view);
            this.view = view;
            name = (TextView) view.findViewById(R.id.license_name);
            image = (ImageView) view.findViewById(R.id.license_image);
            detail = (TextView) view.findViewById(R.id.license_detail);
        }
    }
}
